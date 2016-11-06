import qbs

Project {
    Product {
        name: "sbosubmit"
        type: "application"
        consoleApplication: true
        destinationDirectory: path

        cpp.cxxLanguageVersion: "c++11"
        cpp.warningLevel: "all"
        cpp.treatWarningsAsErrors: false
        cpp.debugInformation: true

        cpp.commonCompilerFlags: [
            "-ggdb3",
            "-Wshadow",
            "-Wpointer-arith",
            "-Wcast-qual",
            "-Wcast-align",
            "-Wconversion",
            "-Wzero-as-null-pointer-constant",
            "-Wuseless-cast",
            "-Wsign-conversion",
            "-Wfloat-conversion",
            "-Wlogical-op",
            "-Wmissing-declarations",
            "-Wredundant-decls"
        ]

        cpp.dynamicLibraries: [
            "archive",
            "curl"
        ]

        cpp.includePaths: [
            "src"
        ]

        files: [
            "src/*.cpp",
            "src/*.h"
        ]

        Depends {
            name: "cpp"
        }

        Group {
            fileTagsFilter: product.type
            qbs.install: false
        }
    }

    minimumQbsVersion: "1.6.0"
}
