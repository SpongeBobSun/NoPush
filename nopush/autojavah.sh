#!/bin/sh
export ProjectPath=$(cd "../$(dirname "$1")"; pwd)
export TargetClassName="sun.bob.nopush.NoPushService"

export SourceFile="${ProjectPath}/nopush/src/main/java"
export TargetPath="${ProjectPath}/nopush/src/main/jni"

cd "${SourceFile}"
javah -d ${TargetPath} -classpath "${SourceFile}" "${TargetClassName}"
echo -d ${TargetPath} -classpath "${SourceFile}" "${TargetClassName}"
