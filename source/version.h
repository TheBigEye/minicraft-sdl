/*
 * version.h - Project version, single source of truth.
 *
 * Stable releases are tagged vX.Y.Z and pre-releases vX.Y.Z-devN.
 * The release workflows (.github/workflows/make-snapshot.yml and
 * make-release.yml) validate the dispatched version against these
 * numbers, so bump MAJOR/MINOR/PATCH here before starting a new
 * X.Y.Z series.
 */

#ifndef SOURCE_VERSION_H_
#define SOURCE_VERSION_H_

#define MAJOR 1
#define MINOR 1
#define PATCH 0

/* VERSION is built from MAJOR/MINOR/PATCH via stringification, e.g.
 * "1" "." "0" "." "0" concatenates into "1.0.0". */
#define VERSION_STRINGIFY_(x) #x
#define VERSION_STRINGIFY(x) VERSION_STRINGIFY_(x)
#define VERSION VERSION_STRINGIFY(MAJOR) "." VERSION_STRINGIFY(MINOR) "." VERSION_STRINGIFY(PATCH)

#endif /* SOURCE_VERSION_H_ */
