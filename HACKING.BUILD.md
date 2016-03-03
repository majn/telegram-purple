# Hacking on the build system

This list was previously maintained as [issue #242](https://github.com/majn/telegram-purple/issues/242), until we realized that this needs proper version-control. (Duh!)

## Exhaustive list of requirements on the build system:

1. `commit.h` must change whenever it disagrees with the current git commit
1. `commit.h` must not change whenever it agrees with the current git commit
1. Must call `cd tgl && ./configure` with appropriate options whenever `tgl/Makefile` is missing, or notices that `./configure` was run. ("Too often" is acceptable as long as `make && make` isn't violated.)
1. Must call `make -C tgl` whenever this might have any effect. ("Too often" is acceptable as long as `make && make` isn't violated.)
1. Any changes due to `make -C tgl` must cause all necessary rebuilds in telegram-purple during the same invocation of make. ("Too often" is acceptable as long as `make && make` isn't violated.)
1. If `tgl/Makefile.in` is missing, explain that we need submodules, and stop.
1. `make -j12` must work fine
1. Must support out-of-CVS builds.

## While ensuring that:

- Must not `-include tgl/Makefile` as it doesn't really support that, and I'm not going to try and convince @vysheng to do that. Also, it's too fragile. You know how he ticks.
- `make && make` must not create any files in the second run (otherwise, `sudo make install` would become hairy)
- Compatibility with other makes

## Approach:

- `commit.h` is a regular file which depends on a .PHONY target `commit`. Building `commit.h` *only* touches `commit.h` if necessary and possible. (=> 1, 2, half of 8)
- Have a target `tgl/Makefile` which depends on `Makefile`. (=> 3)
- The central target `${PRPL_LIBNAME}` shall depend on the .PHONY `submade`, which depends on `tgl/Makefile`, and essentially executes `make -C tgl`. (=> 4, time constraint of 5)
- All objects of telegram-purple depend on `tgl/libs/libtgl.a`. Rationale: if `tgl/libs/libtgl.a` stays the same, then nothing in tgl changed. If `tgl/libs/libtgl.a` changes, then due to lots of black preprocessor magic within tgl, it can't be safely determined which, if any, objects of telegram-purple can be re-used. Also, if tgl got recompiled (~ 1 minute), then tgp can be recompiled, too (~ 5 seconds). (=> dependency constraint of 5)
- Let `tgl/Makefile` depend on `tgl/Makefile.in`, and put the warning into the rule for the latter. (=> 6)
- The above already implies a dependency DAG that is completely known to make; except at one point: let `tgl/libs/libtgl.a` depend on `submade`, without any own code. Now make ensure thread-safety on it's own. (=> 7)
- Bundle commit.h into the origtar (=> other half of 8)

## Side effects:

- Should be ultimately robust. Then again, that's what I thought about the last few attempts, and they didn't last long.
- This reduces the number of .PHONYs drastically, making it easier to read, and making it easier to prove that `make && make` doesn't change anything.
- Even though `submade` should trigger once and only once, this can handle multiple invocations just fine, too.
- All files dependent on `commit.h` need to be mentioned explicitly as dependents in the Makefile. This is ugly, but (as far as I can see) unavoidable. Thankfully, any errors here will get detected by automated testing.
- We currently use only one GNU make specific extension. See https://github.com/majn/telegram-purple/issues/137#issuecomment-167970054
