import qbs 1.0
import qbs.Environment
import qbs.FileInfo

Product {
    name: "watering"
    type: ["application", "hex"]
    qbsSearchPaths: "./"
    Depends { name:"cpp" }
    property string packagesPath: Environment.getEnv('USERPROFILE') + "/.platformio/packages/"
    property string frameworkPath: packagesPath + "framework-arduinoavr/"
    property string toolchainPath: packagesPath + "toolchain-atmelavr/"
    property string libsPath: frameworkPath + "libraries/"
    property string pioLibsPath: ".piolibdeps/"
    cpp.architecture: "avr"
    cpp.positionIndependentCode: false
    cpp.executableSuffix: ".elf"
    cpp.debugInformation: true // passes -g
    cpp.optimization: "small" // passes -Os
    cpp.warningLevel: "all" // passes -W
    cpp.enableExceptions: false // passes -fno-exceptions
    cpp.commonCompilerFlags: [
        "-ffunction-sections",
        "-fdata-sections",
        "-flto",
        "-mmcu=atmega328p"
    ]
    cpp.cFlags: [
        "-fno-fat-lto-objects",
        "-std=gnu11"
    ]
    cpp.cxxFlags: [
        "-fno-threadsafe-statics",
        "-fpermissive",
        "-std=gnu++11"
    ]
    cpp.linkerFlags: [
        "-Os",
        "-mmcu=atmega328p",
        "-Wl,--gc-sections",
        "-flto",
        "-fuse-linker-plugin"
    ]
    cpp.defines: [
        "F_CPU=16000000L",
        "PLATFORMIO=30201",
        "ARDUINO_ARCH_AVR",
        "ARDUINO_AVR_NANO",
        "ARDUINO=10617"
    ]
    cpp.includePaths: [
        frameworkPath + "cores/arduino",
        frameworkPath + "variants/eightanaloginputs",
        frameworkPath + "libraries/__cores__/arduino/Wire/src",
        toolchainPath + "avr/include",
        toolchainPath + "lib/gcc/avr/4.9.2/include",
        toolchainPath + "lib/gcc/avr/4.9.2/include-fixed",
        pioLibsPath + "TimerOne_ID131",
        pioLibsPath + "RTC_ID274/src"
    ]
    files: [
        "src/*"
    ]
    Properties {
        condition: qbs.buildVariant === "debug"
        cpp.defines: outer.concat(["DEBUG=1"])
    }
    Group {
        qbs.install: true
        fileTagsFilter: "application"
    }
    Group {
        name: "arduino"
        prefix: frameworkPath + "cores/arduino/"
        files: ["*.c","*.cpp","*.S","*.h"]
    }
    Group {
        name: "Wire"
        prefix: frameworkPath + "libraries/__cores__/arduino/Wire/src/**/"
        files: ["*.c","*.cpp","*.h"]
    }
    Group {
        name: "libs"
        prefix: pioLibsPath + "**/"
        files: ["*.c","*.cpp","*.h"]
    }
    Rule {
        inputs: ["application"]
        Artifact {
            fileTags: ['hex']
            filePath: input.baseDir + "/" + input.baseName + ".hex"
        }
        prepare: {
            var sizePath = "avr-size";
            var argsSize = [input.filePath];
            var cmdSize = new Command(sizePath, argsSize);
            cmdSize.description = "Size of sections:";
            cmdSize.highlight = "linker";

            var argsConv = ["-O", "ihex", input.filePath, output.filePath];
            var objCopyPath = "avr-objcopy";
            var cmdConv = new Command(objCopyPath, argsConv);
            cmdConv.description = "converting to HEX: " + FileInfo.fileName(input.filePath);
            cmdConv.highlight = "linker";
            return [cmdSize, cmdConv];
        }
    }
}
