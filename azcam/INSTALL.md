# MODS azcam server installation

**Updated: 2025 Oct 15 [rwp/osu]**

Copy from the MODS2025 repository (/make sure there is no existing /home/dts/azcam folder or if so, back it up first:
```shell
cd /home/dts
cp -r ~/MODS2025/azcam .
```
then login as root
```
conda activate /usr/local/conda/envs/py312
cp /home/dts/azcam
python -m pip install -e ./azcam
python -m pip install -e ./azcam-mods
```
