import os
import shutil

import numpy

import azcam
import azcam.utils
import azcam.fits
import azcam.image
import azcam_console.plot
from azcam_console.testers.basetester import Tester


class Defects(Tester):
    """
    Find and quantify image azcam.db.tools["defects"].
    """

    def __init__(self):
        super().__init__("defects")

        # edge mask
        self.edge_size = 0  # number of pixels to exclude on edges
        self.edge_mask_is_valid = 0
        self.number_edgemasked = 0  # number pixels edge masked

        # masks
        self.edge_mask = 0
        self.dark_mask = 0
        self.bright_mask = 0
        self.defects_mask = 0

        self.mask_is_valid = False

        # bright defects
        self.grade_bright_defects = 1
        self.bright_pixel_reject = -1  # reject dark image pixels above this e/pix/sec
        self.brightdefects_datafile = "brightdefects.txt"
        self.brightdefectsreport_rst = "brightdefects"
        self.dark_filename = "dark.fits"  # dark image for bright defects
        self.BrightPixelRejectionMask = "BrightPixelRejectionMask.png"
        self.bright_defects_grade = "FAIL"
        self.allowable_bright_pixels = -1
        self.allowable_bright_defects_per_column = -1  # allowable defects per column
        self.total_bright_pixels = -1  # total pixels with mask

        # dark defects
        self.grade_dark_defects = 1
        self.dark_pixel_reject = -1  # reject dark pixels below this value from mean
        self.darkdefects_datafile = "darkdefects.txt"
        self.darkdefectsreport_file = "darkdefects"
        self.DarkPixelRejectionMask = "DarkPixelRejectionMask.png"
        self.flat_filename = "superflat.fits"  # flat image for dark pixels
        self.dark_defects_grade = "FAIL"
        self.dark_rejected_pixels = -1
        self.allowable_dark_pixels = -1
        self.allowable_dark_defects_per_column = -1  # allowable defects per column
        self.total_dark_pixels = -1  # total pixels with mask

        # total defects
        self.data_file = "defects.txt"
        self.report_file = "defects"
        self.defects_mask_filename = "DefectsMask.fits"
        self.PixelRejectionMask = "PixelRejectionMask.png"
        self.total_rejected_pixels = -1
        self.allowable_bad_pixels = -1  # allowable total bad pixels

        self.report_include_plots = 1  # include plots in report file

    def analyze(self):
        """
        Report on defects from dark and superflat analysis.
        """

        azcam.log("Analyzing total defects")

        dark = azcam.db.tools["dark"]
        superflat = azcam.db.tools["superflat"]

        # create defects mask if does not already exit
        if not self.mask_is_valid:
            self.create_defects_mask()

        self.total_rejected_pixels = (
            dark.bright_rejected_pixels + superflat.dark_rejected_pixels
        )
        azcam.log(f"Total rejected pixels: {self.total_rejected_pixels}")

        if self.allowable_bad_pixels > 0:
            if self.total_rejected_pixels <= self.allowable_bad_pixels:
                self.grade = "PASS"
            else:
                self.grade = "FAIL"

        # define dataset
        self.dataset = {
            "brightdefects_datafile": dark.brightdefects_datafile,
            "bright_defects_grade": dark.bright_defects_grade,
            "allowable_bright_pixels": dark.allowable_bright_pixels,
            "bright_pixel_reject": float(self.bright_pixel_reject),
            "bright_rejected_pixels": float(dark.bright_rejected_pixels),
            "darkdefects_datafile": superflat.darkdefects_datafile,
            "dark_defects_grade": superflat.grade,
            "allowable_dark_pixels": superflat.allowable_dark_pixels,
            "dark_pixel_reject": float(superflat.dark_pixel_reject),
            "dark_rejected_pixels": float(superflat.dark_rejected_pixels),
            "data_file": self.data_file,
            "grade": self.grade,
            "allowable_bad_pixels": self.allowable_bad_pixels,
            "total_rejected_pixels": float(self.total_rejected_pixels),
        }

        # write output files
        self.write_datafile()
        if self.create_reports:
            self.report()

        return

    def create_defects_mask(self):
        """
        Create a single mask from Edge Mask, Bright mask, and Dark mask.
        Execute AFTER dark and bright defects are found.
        """

        dark = azcam.db.tools["dark"]
        superflat = azcam.db.tools["superflat"]

        # combine masks
        self.defects_mask = numpy.ma.logical_or(
            dark.masked_image.mask, superflat.masked_image.mask
        )

        fig = azcam_console.plot.plt.figure()
        fignum = fig.number
        azcam_console.plot.move_window(fignum)
        azcam_console.plot.plt.title("Pixel Rejection Mask")
        implot = azcam_console.plot.plt.imshow(self.defects_mask.astype("uint8"))
        implot.set_cmap("gray")
        azcam_console.plot.plt.show()
        azcam_console.plot.save_figure(fignum, "PixelRejectionMask")

        # write mask as FITS
        maskfile = azcam.image.Image(superflat.superflat_filename)
        maskfile.hdulist[0].header["OBJECT"] = "pixel mask"
        maskfile.assemble(1)  # for parameters
        maskfile.buffer = self.defects_mask
        maskfile.save_data_format = 8
        maskfile.overwrite = 1
        maskfile.write_file(f"{self.defects_mask_filename}", 6)

        # write mask as FITS
        # self.df = azcam.image.Image()
        # # defectsmask.assemble(1)
        # defectsmask.buffer = numpy.ma.getmask(self.defects_mask).astype("uint8")
        # # defectsmask.buffer = self.defects_mask.astype("uint8")
        # defectsmask.save_data_format = 8
        # defectsmask.overwrite = 1
        # defectsmask.write_file(self.defects_mask_filename, 6)
        self.defects_mask_filename = os.path.abspath(self.defects_mask_filename)

        self.mask_is_valid = True  # defects mask in now valid

        return

    def mask_edges(self, masked_array: numpy.array, edge_size: int = -1):
        """
        Mask the edges of the input masked array.

        Args:
            masked_array: input masked array
            edge_size: integer number of pixels closest to edges to mask.
        """

        # edge size
        if edge_size != -1:
            es = edge_size
        else:
            es = self.edge_size

        # set edge pixels as invalid (true)
        nrows, ncols = masked_array.shape
        count = 0
        for y in range(nrows):
            for x in range(ncols):
                if x - es < 0:
                    masked_array.mask[y, x] = 1
                    count += 1
                elif x + es + 1 > ncols:
                    masked_array.mask[y, x] = 1
                    count += 1
                elif y - es < 0:
                    masked_array.mask[y, x] = 1
                    count += 1
                elif y + es + 1 > nrows:
                    masked_array.mask[y, x] = 1
                    count += 1
        self.number_edgemasked = count

        self.edge_mask_is_valid = 1

        return masked_array

    def mask_defects(self, masked_image: numpy.array):
        """
        Apply defects mask to input masked image.

        Args:
            masked_image: input masked image
        """

        dark = azcam.db.tools["dark"]
        superflat = azcam.db.tools["superflat"]

        if dark.is_valid and superflat.is_valid:
            mask = numpy.ma.mask_or(
                dark.masked_image.mask, superflat.masked_image.mask, copy=True
            )
        elif dark.is_valid:
            mask = dark.masked_image.mask
            masked_image.mask = mask
        elif superflat.is_valid:
            mask = superflat.masked_image.mask
            masked_image.mask = mask
        else:
            self.mask_edges(masked_image)

        return masked_image

    def read_defects_mask(self, filename: str = ""):
        """
        Read a defects mask (FITS format).
        """

        if filename == "":
            filename = self.defects_mask_filename

        defectsimage = azcam.image.Image(filename)
        defectsimage.assemble(1)
        self.DefectsImage = defectsimage

        # self.DefectsMask=numpy.ma.masked_invalid(defectsimage.buffer)
        self.defects_mask = numpy.ma.masked_where(
            defectsimage.buffer > 0, defectsimage.buffer
        )  # 0 or 1, not >1

        self.is_valid = True  # defects mask in now valid

        del defectsimage

        return

    def get_defect_coords(self, mask_in):
        """
        Get the coordinates [col,row] of defects, not including edge mask.
        """

        nrows = len(mask_in)
        ncols = len(mask_in[0])

        # mask=numpy.ma.getmask(mask_in)
        coords = mask_in.nonzero()
        lc = len(coords[0])

        coords1 = []
        for i in range(lc):
            row = coords[0][i]
            col = coords[1][i]

            if self.edge_size > 0:
                if row < self.edge_size:
                    pass
                elif row >= (nrows - self.edge_size):
                    pass
                elif col < self.edge_size:
                    pass
                elif col >= (ncols - self.edge_size):
                    pass
                else:
                    coords1.append([col, row])
            else:
                coords1.append([col, row])

        # return [col,row] pairs
        coords2 = sorted(coords1, key=lambda k: [k[0], k[1]])
        return coords2

    def plot_defects_mask(self):
        """
        Plot the defective pixel mask.
        """

        fig = azcam_console.plot.plt.figure()
        fignum = fig.number
        azcam_console.plot.move_window(fignum)
        azcam_console.plot.plt.title("Pixel Rejection Mask")
        implot = azcam_console.plot.plt.imshow(self.defects_mask.astype("uint8"))
        implot.set_cmap("gray")
        azcam_console.plot.plt.show()
        azcam_console.plot.save_figure(fignum, "PixelRejectionMask")

        return

    def report(self):
        """
        Write defects report file for Bright and Dark.
        Run only after all analysis has been completed.
        """

        lines = ["# Total Defects Analysis"]

        lines.append(f"Number of pixels rejected: {self.total_rejected_pixels}  ")
        if self.allowable_bad_pixels > 0:
            lines.append(
                f"Allowable total bad pixels: {int(self.allowable_bad_pixels)}  "
            )
        if self.grade != "UNDEFINED":
            lines.append(f"Total defects grade: {self.grade}  ")

        if self.report_include_plots:
            lines.append(
                f"![Total pixel rejection mask]({os.path.abspath(self.PixelRejectionMask)})  "
            )
            lines.append("*Total pixel rejection mask.*  ")

        # Make report files
        self.write_report(self.report_file, lines)

        return
