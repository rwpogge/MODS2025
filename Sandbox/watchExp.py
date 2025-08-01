from time import time
from time import sleep

def watchExp():
    t0 = time.time()
    efd = azcam.db.exposureflags
    exp = exposure.exposure_flag
    oldExp = -1
    while exp > 0:
        if exp != oldExp:
            dt = time.time() - t0
            statStr = list(filter(lambda key: efd[key]==exp,efd))[0]
            print(f"expstatus: {statStr} [{exp}]  {dt:.3f} sec")
            oldExp = exp
        if exp==1:
            expleft = exposure.get_exposuretime_remaining()
            n = sys.stdout.write(f"\rExptime remaining {expLeft:.3f} sec...")
        exp=exposure.exposure_flag
        sleep(0.02)
    dt = time.time() - t0
    print(f"done: expstatus=IDLE [0]  {dt:.3f} sec")

