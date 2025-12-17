# MODS azcam server installation

**Updated: 2025 Dec 17 [rwp/osu]**

Copy the latest repo from the MODS2025 repository (make sure there is no existing /home/dts/azcam folder or if so, back it up first!):
```shell
cd /home/dts
cp -r ~/MODS2025/azcam .
```
then login as root
```
conda activate /usr/local/conda/envs/py312
cd /home/dts/azcam
python -m pip install -e ./azcam
python -m pip install -e ./azcam-mods
python -m pip install -e ./azcam-console
```
