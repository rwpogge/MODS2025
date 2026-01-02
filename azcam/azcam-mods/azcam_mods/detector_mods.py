"""
MODS focal plane CCD configuration

Updates:
    2025 Jul 10 [rwp/osu] - changed im# -> Q# to match MODS nomenclature
    2025 Nov 21 [mlesser] - changes for working CCD config, including overscan columns
    2026 Jan 01 [rwp/osu] - added 2 pix per line per amp for correct 50 pre-scan pixels
                            instead of 48 pre-scan pixels.  Change 4144->4146
                            and 8288->8292.  Also reassigned ext_name
"""

detector_mods = {
    "name": "MODS",
    "description": "MODS",
    "ref_pixel": [4146.0, 1544.0],
    "format": [8292, 0, 0, 32, 3088, 0, 0, 0, 0],
    "focalplane": [1, 1, 2, 2, [0, 1, 0, 1]],
    "roi": [1, 8292, 1, 3088, 1, 1],
    "ext_position": [[1, 1], [2, 1], [1, 2], [2, 2]],
    "jpg_order": [1, 2, 3, 4],
    "ext_number": [1, 2, 3, 4],
    "det_number": [1, 1, 1, 1],
    "det_position": [
        [1, 1],
        [1, 1],
        [1, 1],
        [1, 1],
    ],
    "amp_position": [
        [1, 1],
        [2, 1],
        [1, 2],
        [2, 2],
    ],
    "amp_pixel_position": [
        [1, 1],
        [8292, 1],
        [1, 3088],
        [8292, 3088],
    ],
    "ext_name": [
        "IM1",
        "IM2",
        "IM3",
        "IM4",
    ],
}
