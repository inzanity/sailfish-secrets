#!/bin/bash
# Matrix Test 26.b: CollectionSecret + GQR + DeviceLock + AccessRelock + NoAccessControl + Non-Privileged-Store + Non-privileged-Access + InstantAccess
# Requires: daemon to be running in --test mode, this test should be run in a non-privileged terminal
groupname=$(id -gn)
if [ "$groupname" == "privileged" ]; then
    echo "        SKIP: Script is being run in privileged terminal!"
    exit 1
else
    if ! secrets-tool --test --get-collection-secret org.sailfishos.secrets.plugin.encryptedstorage.sqlcipher.test TestCollection TestSecret ; then
        echo "        FAIL: Unable to get collection secret!"
        exit 4
    fi
fi
echo "        PASS"
exit 0
