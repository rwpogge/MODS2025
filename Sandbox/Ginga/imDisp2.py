import numpy as np
from astropy.io import fits
from ginga.util import grc
from pathlib import Path

# 1. Connect to the running Ginga instance

gingaHost = 'localhost'
bluePort = 11711  # custom port, set at ginga run by --rcport=11711
redPort = 11712  # custom port, set at ginga run by --rcport=11712

# get a remote viewer

bv = grc.RemoteClient(gingaHost, bluePort)
rv = grc.RemoteClient(gingaHost, redPort)

# 2. Target a specific channel in Ginga (creates it if it doesn't exist)

bc = bv.channel('Image')
rc = rv.channel('Image')

def modsDisp(chan,filePath):
    
    # 3. Open your local FITS file and extract the image data

    fitsPath = Path(filePath)
    rootName = fitsPath.name

    # MODS post-proc raw merged image is in HDU 6

    with fits.open(fitsPath) as hdul:
    
        img_data = hdul[0].data if hdul[0].data is not None else hdul[6].data
        img_hdr = dict(hdul[6].header)

        # Ensure data is converted to float or native NumPy format if necessary
    
        img_data = np.asarray(img_data, dtype=np.float32)

        # 4. Stream the data directly into Ginga's memory
        #    Parameters: (name, numpy_array, format_type, header_dict)

        chan.load_np(fitsPath.name, img_data, 'fits', img_hdr) # was {}

