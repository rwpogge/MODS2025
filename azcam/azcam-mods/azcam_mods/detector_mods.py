"""
MODS focal plane configuration

Updated: 2025 July 10 [rwp/osu]
         changed im# -> Q# to match MODS nomenclature
"""

detector_mods = {
    "name": "MODS",
    "description": "MODS",
    "ref_pixel": [4144.0, 1544.0],
    "format": [8288, 0, 0, 0, 3088, 0, 0, 0, 0],
    "focalplane": [1, 1, 2, 2, [0, 1, 0, 1]],
    "roi": [1, 8288, 1, 3088, 1, 1],
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
        [8288, 1],
        [1, 3088],
        [8288, 3088],
    ],
    "ext_name": [
        "Q1",
        "Q2",
        "Q3",
        "Q4",
    ],
}
