This directory contains source code for the jSMILE tutorial.

You must add your SMILE license key to src/tutorials/App.java, otherwise
the tutorial will not run.

jSMILE jar (usually named jsmile-x.y.z.jar, where x.y.z is the version number)
must be on the Java classpath.

jSMILE will attempt to load its native library (jsmile.dll on Windows, jsmile.so on Linux 
and jsmile.jnilib on macOS). The first attempt is based on the jsmile.native.library system
property. If this property has been set, System.load will be used to load the native libary.
System.load requires the complete path name (not just a directory).
If jsmile.native.library is not set, System.loadLibrary is used. This function searches for 
dynamic libraries in the directories specified in java.library.path property. Note that
java.library.path cannot be set once JVM is running (but jsmile.native.library can, by a call 
to System.setProperty).
