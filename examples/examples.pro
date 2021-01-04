TEMPLATE = subdirs

SUBDIRS += \
    cli

qtHaveModule(quickcontrols2): SUBDIRS += \
    quick
