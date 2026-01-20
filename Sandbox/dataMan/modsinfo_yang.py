#!/usr/bin/env python2.7

import os
import sys
import math
import pprint
import datetime
import pymongo
import bson
import warnings
import pyfits
from collections import OrderedDict
# supress pyfits warning
warnings.resetwarnings()
warnings.filterwarnings('ignore', category=UserWarning, append=True)

# simple command-line argument:
# argv[1]: the input image name (including full path, must be writable)
# argv[2]: verbose mode. default is False.
image = None
verbose = False
if len(sys.argv) > 1:
    image = sys.argv[1]
if len(sys.argv) > 2:
    verbose = True

# check and process some options first
prog = os.path.basename(sys.argv[0])
version = "{} (LBTO)v0.1 [Fri Feb 17 2023]".format(prog)
if image == "--version":
    print version
    sys.exit(0)
elif image == "--help":
    print prog + " --help    => show this help message"
    print prog + " --version => show version number"
    print prog + " <img>     => process <img> to add fits info"
    print prog + " <img> -v  => process <img> to add fits info (verbose mode)"
    sys.exit(0)

# check image is valid
if not image:
    print "No input image, please supply one!"
    sys.exit(1)
if not os.access(image, os.R_OK|os.W_OK):
    print "No read/write access to input image: '{}'".format(image)
    sys.exit(1)
if not os.path.isfile(image):
    print "Input image '{}' is not a regular file!".format(image)
    sys.exit(1)

# some built-in constants
# DMS mongodb credentials:
mongocreds = {'host': 'dms.mountain.lbto.org',
              'port':  27017,
              'username': 'tcs_user',
              'password': 'FenasOwnyo',
              'authSource': 'tcs'}
# MODS camera specs, these should be obtained from the standard
# database configuration collection. if we fail to obtain these
# parameters, then the entire program will fail. we put the current
# default values here just for reference purposes.
# pixel scale is in arcsec/pixel unit
#pixel_scale = {'1': {"R": 0.123, "B": 0.120},
#               '2': {"R": 0.123, "B": 0.120}}
# detector size unit is pixel.
#det_size = {'1': {"R": ((1,8288),(1,3088)),
#                  "B": ((1,8288),(1,3088))},
#            '2': {"R": ((1,8288),(1,3088)),
#                  "B": ((1,8288),(1,3088))}}
## the reference rotation center is relative to the whole detector size
# the coordinate unit is in pixel.
#cam_center = {'1': {"R": [4134.5, 1548.5],
#                    "B": [4143, 1543]},
#              '2': {"R": [4105, 1525],
#                    "B": [4109, 1511]}}
# corrections to the XY plane shift, right now, this is due to
# the init PCS configuration values. but in the future, we could
# also include errors in the camera rotation center measurement etc.
# notice these should be in "mm" unit.
#xy_correction = {'1': {"R": [-1.90, -1.20],
#                       "B": [-1.90, -1.20]},
#                 '2': {"R": [-3.75, -1.00],
#                       "B": [-3.75, -1.00]}}

# a function that retrieves some essential information from an
# input image, it will return a dictionary:
# 't' : the capture time of the input image (from the fits keyword
#       "DATE-OBS"), returned a datetime.datetime object
# 'n' : either "1" or "2" (string) representing which MODS
#       this is extracted from keyword "DETECTOR"
# 'c' : either "R" or "B" (string) representing the MODS channels
#       (red or blue). this is extracted from keyword "CHANNEL"
# 'roi' : two pairs of integers (x1,x2) and (y1,y2), this is the
#       region of interest (sub-frame) the input image has over
#       the entire MODS detector. this information is extracted
#       from the keyword "CCDROI".
# 'xbin' : 1 or 2 (integer), the binning factor along x, extracted
#       from the keyword "CCDXBIN"
# 'ybin' : 1 or 2 (integer), the binning factor along y, extracted
#       from the keyword "CCDYBIN"
# if any of these information is not valid, then returns None 
# instead of a dictionary.
def parse_roi(roi):
    # input roi is a string such as "[2601:5688,1:3088]",
    # output is two pairs of integers such as (2601,5688) and (1,3088)
    # will raise exception if any errors occur
    x, y = roi.strip()[1:-1].split(',')
    x1, x2 = x.strip().split(':')
    y1, y2 = y.strip().split(':')
    return (int(x1),int(x2)), (int(y1),int(y2))

def get_image_info(img):
    try:
        header = pyfits.open(img)[0].header
        # first check if we have a MODS image
        if not header['INSTRUME'].strip().startswith('MODS'):
            return None
        t = header['DATE-OBS']
        t = datetime.datetime.strptime(t,'%Y-%m-%dT%H:%M:%S.%f')
        n = int(header['DETECTOR'].split()[-1].strip())
        c = header['CHANNEL'].strip()[0].upper()
        if n!=1 and n!=2:
            raise ValueError("MODS [n] is wrong, '{}' detected".format(n))
        if c!='B' and c!='R':
            raise ValueError("MODS [c] is wrong, '{}' detected".format(c))
        roi = parse_roi(header['CCDROI'])
        xbin = int(header['CCDXBIN'])
        ybin = int(header['CCDYBIN'])
        if xbin!=1 and xbin!=2:
            raise ValueError("XBin is wrong, '{}' detected".format(xbin))
        if ybin!=1 and ybin!=2:
            raise ValueError("YBin is wrong, '{}' detected".format(ybin))
        return {'t'    : t,
                'n'    : str(n),
                'c'    : c,
                'roi'  : roi,
                'xbin' : xbin,
                'ybin' : ybin}
    except:
        return None

# this function takes a datetime object and queries the DMS tcs database
# for information matching the passed in datetime. specifically, given a
# datetime object 't', this function returns the database record whose
# timestamp is closest to t, but <= t.
# it returns a dictionary containing the following parameters:
# the followings are useful for calculating WCS info
# "PosAngle", 
# "POXCurrent", 
# "POXCurrent", 
# "POYCurrent",
# "PORACurrent",
# "PODECCurrent", 
# the followings are used to indicate non-sidereal tracking info
# "isNonSiderealTarget",
# "target_NSRARateCosDec",
# "target_NSDecRate",
# "target_NStype",
# "target_NSEphemeris",
# all above variables include versions for "L" and "R" sides.
# in case of error, or no database record match, then returns None.
def get_tcs_info(t):
    try:
        # create a mongodb connection
        client = pymongo.MongoClient(**mongocreds)
        db = client.tcs
        info = db.fits_info.find(filter={"timestamp": {"$lte": t}},
                                 sort=[("timestamp",pymongo.DESCENDING)],
                                 limit=1)[0]
        info = utf8_encode(info)
        return {'1': {"PosAngle":     info["L_PosAngle"],
                      "POXCurrent":   info["L_POXCurrent"],
                      "POYCurrent":   info["L_POYCurrent"],
                      "PORACurrent":  info["L_PORACurrent"],
                      "PODECCurrent": info["L_PODECCurrent"],
                      "isNonSiderealTarget":
                                      info["L_isNonSiderealTarget"],
                      "target_NSRARateCosDec":
                                      info["L_target_NSRARateCosDec"],
                      "target_NSDecRate":
                                      info["L_target_NSDecRate"],
                      "target_NStype":info["L_target_NStype"],
                      "target_NSEphemeris":
                                      info["L_target_NSEphemeris"]},
                '2': {"PosAngle":     info["R_PosAngle"],
                      "POXCurrent":   info["R_POXCurrent"],
                      "POYCurrent":   info["R_POYCurrent"],
                      "PORACurrent":  info["R_PORACurrent"],
                      "PODECCurrent": info["R_PODECCurrent"],
                      "isNonSiderealTarget":
                                      info["R_isNonSiderealTarget"],
                      "target_NSRARateCosDec":
                                      info["R_target_NSRARateCosDec"],
                      "target_NSDecRate":
                                      info["R_target_NSDecRate"],
                      "target_NStype":info["R_target_NStype"],
                      "target_NSEphemeris":
                                      info["R_target_NSEphemeris"]}}
    except:
        return None

# this function will query the MODS WCS configuration database and
# return the current pixel scale, camera rotation center,
# the total MODS detector size information, and the X/Y plane
# correction factors.
# in case of error, it returns None
def utf8_encode(d):
    if isinstance(d,unicode) or isinstance(d,bson.Binary):
        return d.encode('utf-8')
    elif isinstance(d,dict):
        return {utf8_encode(k):utf8_encode(v) for k,v in d.items()}
    elif isinstance(d,list):
        return [utf8_encode(i) for i in d]
    else:
        return d

def get_mods_config():
    try:
        # create a mongodb connection
        client = pymongo.MongoClient(**mongocreds)
        db = client.tcs
        config = db.mods_wcs_config.find_one()
        # since pymongo decodes all strings to utf8, we will need
        # to re-encode them after the query
        config = utf8_encode(config)
        return config["pixel_scale"],\
               config["cam_rot_center"],\
               config["det_size"],\
               config["xy_correction"]
    except:
        return None

# this function will translate and scale the given camera rotation
# center to a new value according to the ROI and Binning information.
# this function accepts:
# a camera rotation center (a pair of (x,y) pixel coordinate, float type),
# MODS detector size in x (a pair of (l,r) pixel bounds, int type),
# MODS detector size in y (a pair of (b,t) pixel bounds, int type),
# ROI in x (a pair of (l,r) pixel bounds, int type),
# ROI in y (a pair of (b,t) pixel bounds, int type),
# X Bin factor (1 or 2),
# Y Bin factor (1 or 2),
# and it returns a new camera rotation center (a pair of (x,y) pixel
# coordinate in float type)
# notice we do not perform any validity check inside, it is assumed
# that the parameters given are totally legitimate, if not, then
# this function will return nonsense values.
def trans_scale_cam_ro(cam_ro, det_x, det_y, roi_x, roi_y, bin_x, bin_y):
    rox, roy = cam_ro
    x_shift = roi_x[0] - det_x[0]
    y_shift = roi_y[0] - det_y[0]
    rox -= x_shift
    roy -= y_shift
    rox /= bin_x
    roy /= bin_y
    return (rox, roy)

# this function takes a fits image, and a dictionary of keywords/values,
# it updates the image to include (or modify) the given keywords/values.
# it will throw exceptions if something is wrong (so caller is responsible
# to catch any errors if they desire).
def update_img(img, keys):
    with pyfits.open(img, mode='update') as hdulist:
        header = hdulist[0].header
        for k,v in keys.items():
            header[k] = v

# pox/y_current are in mm unit. we need to multiply ~1.6677 to 
# convert them to arcseconds. this value comes from the number
# ~ 1.6677 "/mm in the focal plane. we also did an empirical 
# calculation of this number based on some real data, and we
# are using this calculated number.
arcsec_per_mm = 1.671224003674374
# this function takes MODS camera information (pixel scale and camera
# rotation center) and telescope pointing information, generates a 
# dictionary of WCS keywords calculated from these information.
def gen_wcs(pixelscale, cam_ro, xy_corr, pt_info):
    cam_rox, cam_roy = cam_ro
    pixelscale_x, pixelscale_y = pixelscale
    corr_x, corr_y = xy_corr
    pos_angle = pt_info['PosAngle']
    pora_current = pt_info['PORACurrent']
    podec_current = pt_info['PODECCurrent']
    pox_current = (pt_info['POXCurrent'] - corr_x) * arcsec_per_mm
    poy_current = (pt_info['POYCurrent'] - corr_y) * arcsec_per_mm
    wcs = OrderedDict()
    wcs['CTYPE1'] = ('RA---TAN',
                     'WCS coordinate type and projection')
    wcs['CTYPE2'] = ('DEC--TAN',
                     'WCS coordinate type and projection')
    wcs['CRPIX1'] = (cam_rox + pox_current/pixelscale_x,
                     'WCS pixel coordinates of the reference point')
    wcs['CRPIX2'] = (cam_roy + poy_current/pixelscale_y,
                     'WCS pixel coordinates of the reference point')
    wcs['CRVAL1'] = (math.degrees(pora_current),
                     'WCS coordinates on the reference point')
    wcs['CRVAL2'] = (math.degrees(podec_current),
                     'WCS coordinates on the reference point')
    # CD1_1 is the partial of first axis coordinate w.r.t X
    # CD1_2 is the partial of first axis coordinate w.r.t Y
    # CD2_1 is the partial of second axis coordinate w.r.t X
    # CD2_2 is the partial of second axis coordinate w.r.t Y
    wcs['CD1_1' ] = (-1.0 * math.cos(pos_angle) * pixelscale_x / 3600,
                     'WCS matrix for scaling and rotation')
    wcs['CD1_2' ] = (math.sin(pos_angle) * pixelscale_y / 3600,
                     'WCS matrix for scaling and rotation')
    wcs['CD2_1' ] = (math.sin(pos_angle) * pixelscale_x / 3600,
                     'WCS matrix for scaling and rotation')
    wcs['CD2_2' ] = (math.cos(pos_angle) * pixelscale_y / 3600,
                     'WCS matrix for scaling and rotation')
    return wcs

# this function simply takes the non-sidereal info from tcs and
# dumps them into an ordered dictionary (with fits comments)
def gen_ns(ns_info):
    ns = OrderedDict()
    ns['NONSIDER'] = (False if ns_info['isNonSiderealTarget']==0 else True,
                      'T if target is non-sidereal')
    ns['NSRARATE'] = (ns_info['target_NSRARateCosDec'],
                      'Differential tracking rate in RA')
    ns['NSDERATE'] = (ns_info['target_NSDecRate'],
                      'Differential tracking rate in DEC')
    ns['NSTYPE']   = (ns_info['target_NStype'],
                      'Non-sidereal type')
    ns['NSEPHFIL'] = (ns_info['target_NSEphemeris'],
                      'Non-sidereal ephemeris file')
    return ns

# main program starts
if __name__ == "__main__":
    add_wcs = True
    add_ns  = True
    if verbose:
        print "Starting: {}".format(version)

    if verbose:
        print "* Reading MODS image: '{}'".format(image)

    img_info = get_image_info(image)

    if not img_info:
        print "==> Error retrieving image information (or not a MODS image)"
        sys.exit(1)

    if verbose:
        print "  ++ image info: "
        print "     => time: {}".format(img_info['t'].isoformat())
        print "     => mods: {}".format(img_info['n'])
        print "     => chan: {}".format(img_info['c'])
        print "     => xroi: {}".format(img_info['roi'][0])
        print "     => yroi: {}".format(img_info['roi'][1])
        print "     => xbin: {}".format(img_info['xbin'])
        print "     => ybin: {}".format(img_info['ybin'])
        print "* Retrieving associated tcs information"

    tcs_info = get_tcs_info(img_info['t'])

    if not tcs_info:
        print "==> Error retrieving tcs information"
        sys.exit(1)

    tcs_info = tcs_info[img_info['n']]
    if verbose:
        print "  ++ associated tcs info:"
        print '\n'.join(
                [' '*5+s for s in pprint.pformat(tcs_info).split('\n')])
        print "* Retrieving MODS WCS parameters"

    config = get_mods_config()

    if not config:
        print "==> Error retrieving MODS WCS parameters"
        add_wcs = False

    if add_wcs:
        pixel_scale, cam_center, det_size, xy_corr = config

        pixel_scale = pixel_scale[img_info['n']][img_info['c']]
        pixelscale_x = pixel_scale * img_info['xbin']
        pixelscale_y = pixel_scale * img_info['ybin']
        cam_center = tuple(cam_center[img_info['n']][img_info['c']])
        det_size = det_size[img_info['n']][img_info['c']]
        det_size_x = (int(det_size[0][0]),int(det_size[0][1]))
        det_size_y = (int(det_size[1][0]),int(det_size[1][1]))
        xy_corr = xy_corr[img_info['n']][img_info['c']]
        if verbose:
            print "  ++ binned pixel scale in x (arcsec/pixel):", pixelscale_x
            print "  ++ binned pixel scale in y (arcsec/pixel):", pixelscale_y
            print "  ++ ref camera rotation center (pixels):", cam_center
            print "  ++ total detector size in x (pixels):", det_size_x
            print "  ++ total detector size in y (pixels):", det_size_y
            print "  ++ correction factor in x (mm):", xy_corr[0]
            print "  ++ correction factor in y (mm):", xy_corr[1]
            print "* Translating/Scaling camera rotation center with ROI and (XY)Bin"

        cam_center = trans_scale_cam_ro(cam_center,
                det_size_x, det_size_y,
                img_info['roi'][0], img_info['roi'][1],
                img_info['xbin'], img_info['ybin'])

        if verbose:
            print "  ++ corrected camera rotation center (pixels):", cam_center
            print "* Calculating WCS data and non-sidereal info"

        wcs = gen_wcs((pixelscale_x,pixelscale_y), 
                      cam_center, xy_corr, tcs_info)
    else:
        wcs = OrderedDict()

    if add_ns:
        ns = gen_ns(tcs_info)
    else:
        ns = OrderedDict()

    if verbose:
        print "  ++ WCS result:"
        wcs_p = {k:v[0] for k,v in wcs.items()}
        print '\n'.join(
                [' '*5+s for s in pprint.pformat(wcs_p).split('\n')])
        print "  ++ NS result:"
        ns_p = {k:v[0] for k,v in ns.items()}
        print '\n'.join(
                [' '*5+s for s in pprint.pformat(ns_p).split('\n')])
        print "* Updating image header"

    new_header = OrderedDict()
    new_header.update(wcs)
    new_header.update(ns)
    update_img(image, new_header)

    if verbose:
        print "All done!"
