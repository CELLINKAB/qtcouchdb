gcov {
    zerocounters.commands = lcov --zerocounters --directory .
    capture.commands += lcov --capture --directory $$PWD --directory $$OUT_PWD --output-file coverage-gcov.info --no-external && \
                        lcov --output-file coverage-gcov.info --remove coverage-gcov.info \'*build*\' \'*.moc*\' \'*tests*\'
    genhtml.commands += genhtml coverage-gcov.info --prefix $$absolute_path($$PWD/..) --output-directory doc/coverage
    coverage.depends += first zerocounters check capture genhtml
    QMAKE_EXTRA_TARGETS += coverage zerocounters capture genhtml
    QT_BUILD_PARTS += tests
    !build_pass:message(Code coverage enabled. Run \'make coverage\' to generate the report.)
}

load(qt_parts)
