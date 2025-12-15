import glob
import os
import shutil

import numpy

import azcam
import azcam.utils
import azcam.fits
import azcam.image
import azcam_console.plot
from azcam_console.plot import plt
from azcam_console.testers.basetester import Tester


class Dark(Tester):
    """
    Dark signal acquisition and analysis.
    """

    def __init__(self):
        super().__init__("dark")

        # acquisition
        self.exposure_type = "dark"
        self.number_images_acquire = 1
        self.exposure_time = -1
        self.overscan_correct = 1  # flag to overscan correct images
        self.zero_correct = 1  # flag to correct with bias residuals

        self.fit_order = 3
        """fit order for overscan correction"""

        self.mean_dark_spec = -1  # spec on mean dark signal
        self.grade_dark_signal = 1
        self.dark_signal_grade = "UNKNOWN"

        # defects
        self.grade_bright_defects = 1
        self.bright_pixel_reject = -1  # reject dark image pixels above this e/pix/sec
        self.brightdefects_datafile = "brightdefects.txt"
        self.brightdefectsreport = "brightdefects"
        self.dark_filename = "dark.fits"  # dark image for bright defects
        self.BrightPixelRejectionMask = "BrightPixelRejectionMask.png"
        self.bright_defects_grade = "UNKNOWN"
        self.bright_rejected_pixels = -1  # number of rejected pixels
        self.allowable_bright_pixels = -1
        self.allowable_bright_defects_per_column = -1  # allowable defects per column
        self.total_bright_pixels = -1  # total pixels with mask

        # analysis outputs
        self.mean_dark_signal = -1  # mean dark signal
        self.median_dark_signal = -1  # median dark signal
        self.sdev_dark_signal = -1  # mean dark signal std
        self.dsnu = -1

        self.dark_image = []
        self.masked_image = 0

        self.dark_rate_measured = -1
        self.means = []
        self.sdev = []
        self.hist_bins = []
        self.temperatures = []
        self.temperature = -999
        self.total_pixels = 0  # total non-masked pixels
        self.hist_values = []
        self.hist_fractions = []
        self.histscale_factor = 3.0

        self.report_dark_per_hour = 1
        self.units_scale = 3600  # 3600 for /hr, 1 for /sec
        self.units_text = "e/pix/hr"

        self.data_file = "dark.txt"
        self.report_file = "dark"
        self.dark_filename = "dark.fits"
        self.bias_filename = "bias.fits"
        self.darksub_filename = "dark_sub.fits"
        self.scaled_dark_filename = "darkscaled.fits"
        self.cumm_hist_plot = "cumm_hist.png"
        self.total_hist_plot = "total_hist.png"
        self.darkimage_plot = "darkimage.png"

        self.report_plots = ["cumm_hist", "total_hist", "darkimage"]
        self.report_include_plots = 1  # include plots in report file

    def acquire(self):
        """
        Acquire dark image sets (1 sec dark, long dark) for a dark current measurement.
        NumberImages is the number of image sets to take.
        ExposureTime is the exposure time of dark image in seconds.
        """

        azcam.log("Acquiring dark sequence")

        exposure, tempcon = azcam_console.utils.get_tools(["exposure", "tempcon"])

        # save pars to be changed
        impars = {}
        azcam.utils.save_imagepars(impars)

        # create new subfolder
        currentfolder, newfolder = azcam_console.utils.make_file_folder("dark")
        azcam.db.parameters.set_par("imagefolder", newfolder)

        # clear device
        exposure.test(0)

        azcam.db.parameters.set_par("imageroot", "dark.")  # for automatic data analysis
        azcam.db.parameters.set_par(
            "imageincludesequencenumber", 1
        )  # use sequence numbers
        azcam.db.parameters.set_par("imageautoname", 0)  # manually set name
        azcam.db.parameters.set_par(
            "imageautoincrementsequencenumber", 1
        )  # inc sequence numbers
        azcam.db.parameters.set_par("imagetest", 0)  # turn off TestImage

        # loop through images
        for imgnum in range(self.number_images_acquire):
            # pre-dark bias
            azcam.db.parameters.set_par("imagetype", "dark")  # for GetFilename
            filename = os.path.basename(exposure.get_filename())
            azcam.log(f"Taking pre-dark image: {filename}")
            temp = tempcon.get_temperatures()
            azcam.log(f"Current temperatures: {temp}")
            exposure.expose(0, "zero", "pre-dark bias image")

            # take dark image
            azcam.db.parameters.set_par("imagetype", "dark")
            filename = os.path.basename(exposure.get_filename())
            azcam.log(
                f"Taking dark image {imgnum + 1}/{self.number_images_acquire} for {self.exposure_time:0.3f} seconds: {filename}"
            )
            temp = tempcon.get_temperatures()
            azcam.log(f"  Current temperatures: {temp}")
            exposure.expose(self.exposure_time, "dark", "dark image")

        # finish
        azcam.utils.restore_imagepars(impars)
        azcam.utils.curdir(currentfolder)
        azcam.log("Dark sequence finished")

        return

    def analyze(self):
        """
        Analyze an exisiting series zeros and darks for dark current measurement.
        """

        azcam.log("Analyzing dark signal")

        # reset filenames
        self.data_file = "dark.txt"
        self.report_file = "dark"
        self.dark_filename = "dark.fits"
        self.darksub_filename = "dark_sub.fits"
        self.scaled_dark_filename = "darkscaled.fits"
        self.cumm_hist_plot = "cumm_hist.png"
        self.darkimage_plot = "darkimage.png"

        self.temperatures = []
        self.means = []
        self.gains = []
        rootname = "dark."
        subfolder = "analysis"

        if self.report_dark_per_hour == 1:
            self.units_scale = 3600
            self.units_text = "e/pix/hr"
        else:
            self.units_scale = 1
            self.units_text = "e/pix/sec"

        # copy files to analysis subfolder
        azcam.log("Making copy of image files for analysis")
        startingfolder, subfolder = azcam_console.utils.make_file_folder(subfolder)
        for filename in glob.glob(os.path.join(startingfolder, "*.fits")):
            shutil.copy(filename, subfolder)
        currentfolder = azcam.utils.curdir(subfolder)  # move to analysis folder

        # analyze a sequence
        _, StartingSequence = azcam_console.utils.find_file_in_sequence(rootname)

        # get gain
        self.system_gain = azcam.db.tools["gain"].get_system_gain()

        # get bias image
        zerofilename = rootname + f"{StartingSequence:04d}"
        zerofilename = os.path.join(currentfolder, zerofilename) + ".fits"
        num_exts, _, _ = azcam.fits.get_extensions(zerofilename)
        self._numchans = max(1, num_exts)
        bin1 = azcam.fits.get_keyword(zerofilename, "CCDBIN1")
        bin2 = azcam.fits.get_keyword(zerofilename, "CCDBIN2")
        binned = int(bin1) * int(bin2)

        # get lists of images
        biaslist = []
        darklist = []
        seq = StartingSequence  # pairs are bias then dark
        while True:
            df = os.path.join(currentfolder, rootname) + f"{seq:04d}.fits"
            if os.path.exists(df):
                biaslist.append(df)
                seq += 1
            else:
                break
            df = os.path.join(currentfolder, rootname) + f"{seq:04d}.fits"
            if os.path.exists(df):
                darklist.append(df)
                seq += 1
            else:
                break
        numdarks = len(darklist)

        # median combine all bias images
        masterbias = self.bias_filename
        if numdarks == 1:
            s = f"One bias image found: {darklist[0]}"
            shutil.copyfile(darklist[0], masterbias)
            if self.overscan_correct:
                azcam.fits.colbias(masterbias, fit_order=self.fit_order)
        else:
            azcam.fits.combine(
                biaslist,
                masterbias,
                "median",
                overscan_correct=self.overscan_correct,
                fit_order=self.fit_order,
                datatype="float32",
            )
            s = f"Number of bias images combined into {masterbias}: {numdarks}"
        azcam.log(s)

        # median combine all dark images
        masterdark = self.dark_filename
        if numdarks == 1:
            s = f"One dark image found: {darklist[0]}"
            shutil.copyfile(darklist[0], masterdark)
            if self.overscan_correct:
                azcam.fits.colbias(masterdark, fit_order=self.fit_order)
        else:
            azcam.fits.combine(
                darklist,
                masterdark,
                "median",
                overscan_correct=self.overscan_correct,
                fit_order=self.fit_order,
                datatype="float32",
            )
            s = f"Number of dark images combined into {masterdark}: {numdarks}"
        azcam.log(s)

        azcam.fits.sub(masterdark, masterbias, self.darksub_filename, "float32")

        # create dark azcam image
        self.dark_image = azcam.image.Image(self.darksub_filename)

        # get header info
        exptime = float(azcam.fits.get_keyword(masterdark, "EXPTIME"))
        try:
            self.temperature = float(azcam.fits.get_keyword(masterdark, "CAMTEMP"))
        except Exception:
            self.temperature = -999

        # set scale from gain
        history = azcam.fits.get_history(masterdark)
        if "SCALED" not in history:
            azcam.log("Scale by gain values")
            self.dark_image.set_scaling(self.system_gain, None)

        # trim and assemble dark for histogram
        self.dark_image.assemble(1)

        # scale darkimage by exposure time and binning to get electrons per pixel per second
        self.dark_image.buffer = self.dark_image.buffer / binned / exptime

        # save scaled image as FITS
        self.dark_image.overwrite = 1
        self.dark_image.save_data_format = -32
        self.dark_image.write_file(self.scaled_dark_filename, 6)

        # create masked array
        self.masked_image = numpy.ma.array(self.dark_image.buffer, mask=False)
        defects = azcam.db.tools["defects"]
        defects.mask_edges(self.masked_image)

        # reject bright pixels from dark signal analysis
        if self.bright_pixel_reject != -1:
            s = f"Rejecting bright pixels above {self.bright_pixel_reject:0.03f} e/pix/sec"
            azcam.log(s)
            self.masked_image = numpy.ma.masked_where(
                self.masked_image > self.bright_pixel_reject,
                self.masked_image,
                copy=False,
            )
            self.bright_rejected_pixels = (
                self.masked_image.size
                - numpy.ma.count(self.masked_image)
                - defects.number_edgemasked
            )
            if self.allowable_bright_pixels > 0:
                azcam.log(
                    f"Spec for allowable bright pixels is {int(self.allowable_bright_pixels)}"
                )
            s = f"Number of bright pixels rejected: {int(self.bright_rejected_pixels)}"
            azcam.log(s)

        # get valid data as 1D array
        self.validdata = self.masked_image.compressed()

        self.mean_dark_signal = numpy.ma.mean(self.masked_image)
        self.sdev_dark_signal = numpy.ma.std(self.masked_image)
        self.median_dark_signal = numpy.ma.median(self.masked_image)

        # PASS or FAIL on mean dark signal if specified
        azcam.log(
            f"Mean dark signal is {(self.mean_dark_signal*self.units_scale):0.01f} {self.units_text}"
        )
        if self.median_dark_signal != -1:
            azcam.log(
                f"Median dark signal is {(self.median_dark_signal*self.units_scale):0.01f} {self.units_text}"
            )
        if self.mean_dark_spec != -1:
            if self.mean_dark_signal > self.mean_dark_spec:
                self.dark_signal_grade = "FAIL"
            else:
                self.dark_signal_grade = "PASS"
        if self.mean_dark_spec > 0:
            azcam.log(
                f"Spec for mean dark signal is {(self.mean_dark_spec*self.units_scale):0.01f} {self.units_text}"
            )

        if self.grade_dark_signal:
            azcam.log(f"Dark signal grade = {self.dark_signal_grade}")
        else:
            self.dark_signal_grade = "UNDEFINED"

        # replace dark_filename with darksub_filename
        os.remove(self.dark_filename)
        os.rename(self.darksub_filename, self.dark_filename)

        if self.create_plots:
            self.plot_dark_signal()

        # copy processed dark file starting folder
        if startingfolder != subfolder:
            shutil.copy(os.path.abspath(self.dark_filename), startingfolder)
            shutil.copy(os.path.abspath(self.scaled_dark_filename), startingfolder)
            if self.create_plots:
                shutil.copy(os.path.abspath(self.darkimage_plot), startingfolder)
                shutil.copy(os.path.abspath(self.cumm_hist_plot), startingfolder)
                shutil.copy(os.path.abspath(self.total_hist_plot), startingfolder)

        # set absolute filenames
        azcam.utils.curdir(startingfolder)
        self.dark_filename = os.path.abspath(self.dark_filename)
        self.scaled_dark_filename = os.path.abspath(self.scaled_dark_filename)
        self.darkimage_plot = os.path.abspath(self.darkimage_plot)
        self.cumm_hist_plot = os.path.abspath(self.cumm_hist_plot)

        # save dark mask
        fig = azcam_console.plot.plt.figure()
        fignum = fig.number
        azcam_console.plot.move_window(fignum)
        azcam_console.plot.plt.title("Bright Pixel Rejection Mask")
        self.bright_mask = numpy.ma.getmask(self.masked_image).astype("uint8")
        implot = azcam_console.plot.plt.imshow(self.bright_mask)
        implot.set_cmap("gray")
        azcam_console.plot.plt.show()
        azcam_console.plot.save_figure(fignum, "BrightPixelRejectionMask")

        # write mask as FITS
        maskfile = azcam.image.Image(self.dark_filename)
        maskfile.hdulist[0].header["OBJECT"] = "bright pixel mask"
        maskfile.assemble(1)  # for parameters
        maskfile.buffer = self.bright_mask
        maskfile.save_data_format = 8
        maskfile.overwrite = 1
        maskfile.write_file(f"{self.BrightPixelRejectionMask}.fits", 6)

        # grade bright defects
        if self.grade_bright_defects:
            if self.allowable_bright_pixels != -1:
                if self.bright_rejected_pixels >= self.allowable_bright_pixels:
                    self.bright_defects_grade = "FAIL"
                else:
                    self.bright_defects_grade = "PASS"
                s = "Bright defects grade = %s" % self.bright_defects_grade
                azcam.log(s)

        # write dark signal report
        if self.create_reports:
            self.create_dark_signal_report()
            self.create_bright_defects_report()

        # define dark signal dataset
        self.dataset = {
            "data_file": self.data_file,
            "dark signal grade": self.dark_signal_grade,
            "temperature": self.temperature,
            "total_pixels": float(self.total_pixels),
            "mean_dark_signal": float(self.mean_dark_signal),
            "dark_rate_measured": float(self.dark_rate_measured),
            "hist_bins": numpy.array(self.hist_bins).tolist(),
            "hist_fractions": numpy.array(self.hist_fractions).tolist(),
            "hist_values": numpy.array(self.hist_values).tolist(),
            # "dsnu": numpy.array(self.dsnu).tolist(),
        }

        # write output files
        self.write_datafile()

        # finish
        self.is_valid = True

        return

    def plot_dark_signal(self):
        """
        Plot analysis results.
        """

        if self._numchans == 1:
            plotconfig = (1, 1)
        elif self._numchans == 2:
            plotconfig = (1, 2)
        elif self._numchans == 4:
            plotconfig = (2, 2)
        else:
            raise azcam.exceptions.AzcamError("missing subplot configuration")

        # plot dark image
        fig = plt.figure()
        fignum = fig.number
        azcam_console.plot.move_window(fignum)
        azcam_console.plot.plot_image(self.dark_image, "sdev", 10.0)
        plt.title("Combined Dark Image")
        plt.show()
        azcam_console.plot.save_figure(fignum, self.darkimage_plot)

        for chan in range(self._numchans):
            # plt.subplot(plotconfig[0], plotconfig[1], chan + 1)
            # ax = plt.gca()

            min1 = self.dark_image.data[chan].min()
            max1 = self.dark_image.data[chan].max()
            counts, bins = numpy.histogram(
                self.dark_image.data[chan], int(max1) - int(min1) + 1, density=True
            )
            step = bins[1] - bins[0]
            self.counts = counts
            self.bins = bins

        # plot cummulative histogram
        fig = plt.figure()
        fignum = fig.number
        azcam_console.plot.move_window(fignum)
        plt.hist(
            (self.validdata * self.units_scale),
            bins="auto",
            density=True,
            histtype="step",
            cumulative=True,
        )
        ax = plt.gca()
        ax.set_xlabel(f"Dark Signal [{self.units_text}]")
        ax.set_ylabel("Fraction")

        if self.temperature != -999:
            labels = [f"Mean Temp = {self.temperature:.1f} C"]
            ax.annotate(
                labels[0],
                xy=(0.65, 0.95),
                xycoords="axes fraction",
                horizontalalignment="left",
                verticalalignment="top",
            )

        if self.mean_dark_spec > 0:
            plt.axvline(
                x=self.mean_dark_spec * self.units_scale,
                linestyle="dashed",
                color="red",
                label="Mean spec",
            )

        plt.axvline(
            x=self.mean_dark_signal * self.units_scale,
            linestyle="solid",
            color="red",
            label="Mean",
        )

        if self.median_dark_signal > 0:
            plt.axvline(
                x=self.median_dark_signal * self.units_scale,
                linestyle="dashdot",
                color="black",
                label="Median",
            )

        plt.legend(loc="lower right")
        plt.title("Dark Signal Cummulative Histogram")
        plt.xlim(
            [
                0,
                self.mean_dark_signal * 3 * self.units_scale,
            ]
        )
        plt.show()
        azcam_console.plot.save_figure(fignum, self.cumm_hist_plot)

        # plot total histogram
        fig = plt.figure()
        fignum = fig.number
        azcam_console.plot.move_window(fignum)
        plt.hist(
            (self.validdata * self.units_scale),
            bins="auto",
            density=1,
            histtype="step",
            cumulative=False,
        )
        ax = plt.gca()
        ax.set_xlabel(f"Dark Signal [{self.units_text}]")
        ax.set_ylabel("Pixels")
        plt.xlim(
            [
                0,
                self.mean_dark_signal * self.histscale_factor * self.units_scale,
            ]
        )
        plt.title("Dark Signal Histogram")
        plt.show()
        azcam_console.plot.save_figure(fignum, self.total_hist_plot)

        return

    def create_dark_signal_report(self):
        """
        Create dark signal report file.
        """

        lines = ["# Dark Signal Analysis", ""]

        s = f"Mean dark signal is {self.units_scale*self.mean_dark_signal:0.03f} {self.units_text}  "
        lines.append(s)

        s = f"Median dark signal is {self.units_scale*self.median_dark_signal:0.03f} {self.units_text}  "
        lines.append(s)

        if self.mean_dark_spec > 0:
            s = f"Dark signal spec is {self.units_scale*self.mean_dark_spec:0.03f} {self.units_text}  "
            lines.append(s)

        if self.grade_dark_signal:
            s = f"Dark signal grade = {self.dark_signal_grade}  "
            lines.append(s)

        lines.append("")
        if "cumm_hist" in self.report_plots:
            lines.append(
                f"![Cumulative Histogram]({os.path.abspath(self.cumm_hist_plot)})  "
            )
            lines.append("*Cumulative Dark Signal Histogram.*  ")

        if "total_hist" in self.report_plots:
            lines.append(
                f"![Total Histogram]({os.path.abspath(self.total_hist_plot)})  "
            )
            lines.append("*Total Dark Signal Histogram.*  ")

        if "darkimage" in self.report_plots:
            lines.append(f"![Dark Image]({os.path.abspath(self.darkimage_plot)})  ")
            lines.append("*Dark Image.*  ")

        # Make report files
        self.write_report(self.report_file, lines)

        return

    def create_bright_defects_report(self):
        """
        Write bright defects report file.
        """

        lines = ["# Bright Defects Analysis"]

        lines.append(
            f"Rejecting bright pixels above {self.bright_pixel_reject:0.03f} e/pix/sec  "
        )
        lines.append(f"Number of bright rejected: {self.bright_rejected_pixels}  ")
        lines.append(f"Allowable bad pixels: {int(self.allowable_bright_pixels)}  ")

        # add plots
        if self.report_include_plots:
            lines.append(
                f"![Bright pixel rejection mask]({os.path.abspath(self.BrightPixelRejectionMask)})  "
            )
            lines.append("*Bright pixel rejection mask.*  ")

        # Make report files
        self.write_report(self.brightdefectsreport, lines)

        return
