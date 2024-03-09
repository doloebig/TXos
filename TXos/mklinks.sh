#!/bin/bash
# 
# Create links for all cpp and header files from ../src hierarchy
#

LOCALCONFIGTEMPLATE=TXosLocalConfig.template
LOCALCONFIG=TXosLocalConfig.h

SOURCE="../src ../src/controls ../src/modules ../src/output ../src/TextUI atmega2560"

sh rmlinks.sh

for SOURCEDIR in $SOURCE ; do
    echo Link from $SOURCEDIR

    for FILE in $SOURCEDIR/*.cpp ; do
        TARGET=`basename $FILE`
        echo "  " $FILE " => " $TARGET
        ln -s $FILE $TARGET
    done

    for FILE in $SOURCEDIR/*.h ; do
        TARGET=`basename $FILE`
        if [ $TARGET != $LOCALCONFIG ]; then
            echo "  " $FILE " => " $TARGET
            ln -s $FILE $TARGET
        fi
    done
done

rm Arduino.h TXosTest.cpp TXosUnitTest.cpp TXosUnittestConfig.h

mv TXos.cpp TXos.ino

if [ -f $LOCALCONFIG ]; then
    echo Local config exists. Not overwritten.
else
    cp $LOCALCONFIGTEMPLATE $LOCALCONFIG
    echo
    echo Local config does not exist.
    echo A new $LOCALCONFIG has been generated by copying the template file.
    echo Please modify your local config to your needs.
fi

