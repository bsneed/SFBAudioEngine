// swift-tools-version:5.3
import PackageDescription


let package = Package(
    name: "SFBAudioEngine",
    platforms: [
        .macOS(.v10_14), .iOS(.v13), .tvOS(.v13)
    ],
    products: [
        // Products define the executables and libraries a package produces, and make them visible to other packages.
        .library(
            name: "SFBAudioEngine",
            targets: [
                "SFBAudioEngine"
            ])
    ],
    dependencies: [
        // Dependencies declare other packages that this package depends on.
    ],
    targets: [
        .binaryTarget(
            name: "SFBAudioEngine",
            path: "build/SFBAudioEngine.xcframework"
        ),
        .binaryTarget(
            name: "dumb",
            path: "XCFrameworks/dumb.xcframework"
        ),
        .binaryTarget(
            name: "FLAC",
            path: "XCFrameworks/FLAC.xcframework"
        ),
        .binaryTarget(
            name: "lame",
            path: "XCFrameworks/lame.xcframework"
        ),
        .binaryTarget(
            name: "MAC",
            path: "XCFrameworks/MAC.xcframework"
        ),
        .binaryTarget(
            name: "mpc",
            path: "XCFrameworks/mpc.xcframework"
        ),
        .binaryTarget(
            name: "mpg123",
            path: "XCFrameworks/mpg123.xcframework"
        ),
        .binaryTarget(
            name: "ogg",
            path: "XCFrameworks/ogg.xcframework"
        ),
        .binaryTarget(
            name: "opus",
            path: "XCFrameworks/opus.xcframework"
        ),
        .binaryTarget(
            name: "sndfile",
            path: "XCFrameworks/sndfile.xcframework"
        ),
        .binaryTarget(
            name: "speex",
            path: "XCFrameworks/speex.xcframework"
        ),
        .binaryTarget(
            name: "taglib",
            path: "XCFrameworks/taglib.xcframework"
        ),
        .binaryTarget(
            name: "tta-cpp",
            path: "XCFrameworks/tta-cpp.xcframework"
        ),
        .binaryTarget(
            name: "vorbis",
            path: "XCFrameworks/vorbis.xcframework"
        ),
        .binaryTarget(
            name: "wavpack",
            path: "XCFrameworks/wavpack.xcframework"
        ),
    ]
)
