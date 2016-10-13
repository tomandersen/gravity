How I build it.
———————————

Git for the main repo (src has platform agnostic source)

Then in a libs folder above that 

cd libs

ZFP - compression
Toms-MacBook-Pro:libs tandersen$ git clone git@github.com:LLNL/zfp.git
Toms-MacBook-Pro:libs tandersen$ cd zfp
Toms-MacBook-Pro:zfp tandersen$ make

A static library is created.

