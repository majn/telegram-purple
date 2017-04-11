PLUGIN_TESTS:=probetest loadtest
PLUGIN_TEST_BINS:=$(addprefix test/bin/,${PLUGIN_TESTS})

test/bin:
	mkdir -p $@

test/tmp/%:
	mkdir -p $@

test/bin/%: test/%.c test/bin bin/telegram-purple.so
	${CC} ${CFLAGS} ${CPPFLAGS} -I ${srcdir}/tgl -o $@ $< ${LDFLAGS}

.PHONY: ${PLUGIN_TESTS}
${PLUGIN_TESTS}: %: test/bin/% test/tmp/user
	$< bin/telegram-purple.so

.PHONY: check
check: ${PLUGIN_TESTS} test/tmp/user
	@echo "'make check' passed."

.PHONY: recheck
recheck: clean-test check

.PHONY: clean-test
clean-test:
	rm -rf test/bin test/tmp
