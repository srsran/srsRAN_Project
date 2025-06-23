# SPDX-FileCopyrightText: 2023-2025 Jochem Rutgers
#
# SPDX-License-Identifier: MIT

# Configuring git info
configure_file(
  "${CMAKE_SOURCE_DIR}/cmake/modules/build_info.cmake.in"
  "${CMAKE_BINARY_DIR}/cmake/modules/build_info.cmake"
  @ONLY
)
include("${CMAKE_BINARY_DIR}/cmake/modules/build_info.cmake")

# Sets the given variable to a unique SPDIXID-compatible value.
function(sbom_spdxid)
	set(options)
	set(oneValueArgs VARIABLE CHECK)
	set(multiValueArgs HINTS)

	cmake_parse_arguments(
		SBOM_SPDXID "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
	)
	if(SBOM_SPDXID_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Unknown arguments: ${SBOM_SPDXID_UNPARSED_ARGUMENTS}")
	endif()

	if("${SBOM_SPDXID_VARIABLE}" STREQUAL "")
		message(FATAL_ERROR "Missing VARIABLE")
	endif()

	if("${SBOM_SPDXID_CHECK}" STREQUAL "")

		foreach(_hint IN LISTS SBOM_SPDXID_HINTS)
			string(REGEX REPLACE "[^a-zA-Z0-9]+" "-" _id "${_hint}")
			string(REGEX REPLACE "-+$" "" _id "${_id}")
			if(NOT "${_id}" STREQUAL "")
				set(_id "${_id}${_suffix}")
				break()
			endif()
		endforeach()

		if("${_id}" STREQUAL "")
			set(_id "SPDXRef${_suffix}")
		endif()
	else()
		set(_id "${SBOM_SPDXID_CHECK}")
	endif()

	if(NOT "${_id}" MATCHES "^SPDXRef-[-a-zA-Z0-9]+$")
		message(FATAL_ERROR "Invalid SPDXID \"${_id}\"")
	endif()

	set(${SBOM_SPDXID_VARIABLE}
	    "${_id}"
	    PARENT_SCOPE
	)
endfunction()

# Starts SBOM generation. Call sbom_add() and friends afterwards. End with sbom_finalize(). Input
# files allow having variables and generator expressions.
function(sbom_generate)
	set(options)
	set(oneValueArgs
	    OUTPUT
	    LICENSE
	    COPYRIGHT
	    PROJECT
	    SUPPLIER
	    SUPPLIER_URL
	    NAMESPACE
	    DOWNLOAD_URL
	    EXTREF
	    OSV_QUERY
	)
	set(multiValueArgs INPUT)
	cmake_parse_arguments(
		SBOM_GENERATE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
	)
	if(SBOM_GENERATE_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Unknown arguments: ${SBOM_GENERATE_UNPARSED_ARGUMENTS}")
	endif()

	string(TIMESTAMP NOW_UTC UTC)

	if("${SBOM_GENERATE_OUTPUT}" STREQUAL "")
		set(SBOM_GENERATE_OUTPUT
		    "\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/${PROJECT_NAME}-sbom-${GIT_COMMIT_HASH}.spdx"
		)
	endif()

	if("${SBOM_GENERATE_LICENSE}" STREQUAL "")
		set(SBOM_GENERATE_LICENSE "NOASSERTION")
	endif()

	if("${SBOM_GENERATE_DOWNLOAD_URL}" STREQUAL "")
		set(SBOM_GENERATE_DOWNLOAD_URL "NOASSERTION")
	endif()

	if("${SBOM_GENERATE_PROJECT}" STREQUAL "")
		set(SBOM_GENERATE_PROJECT "${PROJECT_NAME}")
	endif()

	# The Package- prefix should not be required by NTIA, but the ntia-conformance-checker 2.0.0
	# seems to check for it. Probably a bug, but add a workaround anyway.
	string(REGEX MATCH "^Package-" PACKAGE_PREFIX_MATCH ${SBOM_GENERATE_PROJECT})
	if("${PACKAGE_PREFIX_MATCH}" STREQUAL "")
		string(PREPEND SBOM_GENERATE_PROJECT "Package-")
	endif()

	if("${SBOM_GENERATE_SUPPLIER}" STREQUAL "")
		set(SBOM_GENERATE_SUPPLIER "${SBOM_SUPPLIER}")
	elseif("${SBOM_SUPPLIER_URL}" STREQUAL "")
		set(SBOM_SUPPLIER
		    "${SBOM_GENERATE_SUPPLIER}"
		    CACHE STRING "SBOM supplier"
		)
	endif()

	if("${SBOM_GENERATE_COPYRIGHT}" STREQUAL "")
		# There is a race when building at New Year's Eve...
		string(TIMESTAMP NOW_YEAR "%Y" UTC)
		set(SBOM_GENERATE_COPYRIGHT "${NOW_YEAR} ${SBOM_GENERATE_SUPPLIER}")
	endif()

	if("${SBOM_GENERATE_SUPPLIER_URL}" STREQUAL "")
		set(SBOM_GENERATE_SUPPLIER_URL "${SBOM_SUPPLIER_URL}")
		if("${SBOM_GENERATE_SUPPLIER_URL}" STREQUAL "")
			set(SBOM_GENERATE_SUPPLIER_URL "${PROJECT_HOMEPAGE_URL}")
		endif()
	elseif("${SBOM_SUPPLIER_URL}" STREQUAL "")
		set(SBOM_SUPPLIER_URL
		    "${SBOM_GENERATE_SUPPLIER_URL}"
		    CACHE STRING "SBOM supplier URL"
		)
	endif()

	if("${SBOM_GENERATE_NAMESPACE}" STREQUAL "")
		set(SBOM_GENERATE_NAMESPACE
		    "${SBOM_GENERATE_SUPPLIER_URL}/spdxdocs/${PROJECT_NAME}-${GIT_COMMIT_HASH}"
		)
	endif()

	if("${SBOM_GENERATE_EXTREF}" STREQUAL "")
		if(NOT DEFINED GIT_COMMIT_HASH OR "${GIT_COMMIT_HASH}" STREQUAL "")
			set(LENGTH 0)
		else()
			string(LENGTH ${GIT_COMMIT_HASH} _len)
		endif()

		if(NOT "${SBOM_GENERATE_DOWNLOAD_URL}" STREQUAL ""
		   AND NOT "${SBOM_GENERATE_DOWNLOAD_URL}" STREQUAL "NOASSERTION"
		)
			set(SBOM_GENERATE_EXTREF
			    "PACKAGE-MANAGER purl pkg:generic/${PROJECT_NAME}@${GIT_COMMIT_HASH}?download_url=${SBOM_GENERATE_DOWNLOAD_URL}"
			)
		elseif(_len GREATER 40)
			set(SBOM_GENERATE_EXTREF
			    "PERSISTENT-ID gitoid gitoid:commit:sha256:${GIT_COMMIT_HASH}"
			)
		else()
			set(SBOM_GENERATE_EXTREF
			    "PERSISTENT-ID gitoid gitoid:commit:sha1:${GIT_COMMIT_HASH}"
			)
		endif()
	endif()

	string(REGEX REPLACE "[^A-Za-z0-9.]+" "-" SBOM_GENERATE_PROJECT "${SBOM_GENERATE_PROJECT}")
	string(REGEX REPLACE "-+$" "" SBOM_GENERATE_PROJECT "${SBOM_GENERATE_PROJECT}")
	# Prevent collision with other generated SPDXID with -[0-9]+ suffix.
	string(REGEX REPLACE "-([0-9]+)$" "\\1" SBOM_GENERATE_PROJECT "${SBOM_GENERATE_PROJECT}")

	install(
		CODE "
		message(STATUS \"Installing: ${SBOM_GENERATE_OUTPUT}\")
		set(SBOM_EXT_DOCS)
		file(WRITE \"${PROJECT_BINARY_DIR}/sbom/sbom.spdx.in\" \"\")
		"
	)

	file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/sbom)

	if("${SBOM_GENERATE_INPUT}" STREQUAL "")
		if("${SBOM_GENERATE_SUPPLIER}" STREQUAL "")
			message(FATAL_ERROR "Specify a SUPPLIER, or set SBOM_SUPPLIER")
		endif()

		if("${SBOM_GENERATE_SUPPLIER_URL}" STREQUAL "")
			message(FATAL_ERROR "Specify a SUPPLIER_URL, or set SBOM_SUPPLIER_URL")
		endif()

		set(_f "${CMAKE_CURRENT_BINARY_DIR}/SPDXRef-DOCUMENT.spdx.in")

		if(CMAKE_VERSION VERSION_LESS 3.14)
			get_filename_component(doc_name "${SBOM_GENERATE_OUTPUT}" NAME_WE)
		else()
			get_filename_component(doc_name "${SBOM_GENERATE_OUTPUT}" NAME_WLE)
		endif()

		file(
			GENERATE
			OUTPUT "${_f}"
			CONTENT
				"SPDXVersion: SPDX-2.3
DataLicense: CC0-1.0
SPDXID: SPDXRef-DOCUMENT
DocumentName: ${doc_name}
DocumentNamespace: ${SBOM_GENERATE_NAMESPACE}\${SBOM_EXT_DOCS}
Creator: Organization: ${SBOM_GENERATE_SUPPLIER}
Creator: Tool: cmake-sbom
CreatorComment: <text>This SPDX document was created from CMake ${CMAKE_VERSION}, using cmake-sbom
from https://github.com/DEMCON/cmake-sbom</text>
Created: ${NOW_UTC}

PackageName: ${CMAKE_CXX_COMPILER_ID}
SPDXID: SPDXRef-compiler
PackageVersion: ${CMAKE_CXX_COMPILER_VERSION}
PackageDownloadLocation: NOASSERTION
PackageLicenseConcluded: NOASSERTION
PackageLicenseDeclared: NOASSERTION
PackageCopyrightText: NOASSERTION
PackageSupplier: Organization: Anonymous
FilesAnalyzed: false
PackageSummary: <text>The compiler as identified by CMake, running on ${CMAKE_HOST_SYSTEM_NAME} (${CMAKE_HOST_SYSTEM_PROCESSOR})</text>
PrimaryPackagePurpose: APPLICATION
Relationship: SPDXRef-compiler BUILD_DEPENDENCY_OF SPDXRef-${SBOM_GENERATE_PROJECT}
RelationshipComment: <text>SPDXRef-${SBOM_GENERATE_PROJECT} is built by compiler ${CMAKE_CXX_COMPILER_ID} (${CMAKE_CXX_COMPILER}) version ${CMAKE_CXX_COMPILER_VERSION}</text>

PackageName: ${PROJECT_NAME}
SPDXID: SPDXRef-${SBOM_GENERATE_PROJECT}
ExternalRef: ${SBOM_GENERATE_EXTREF}
PackageVersion: ${GIT_COMMIT_HASH}
PackageSupplier: Organization: ${SBOM_GENERATE_SUPPLIER}
PackageDownloadLocation: ${SBOM_GENERATE_DOWNLOAD_URL}
PackageLicenseConcluded: ${SBOM_GENERATE_LICENSE}
PackageLicenseDeclared: ${SBOM_GENERATE_LICENSE}
PackageCopyrightText: ${SBOM_GENERATE_COPYRIGHT}
PackageHomePage: ${SBOM_GENERATE_SUPPLIER_URL}
PackageComment: <text>Built by CMake ${CMAKE_VERSION} with ${CMAKE_BUILD_TYPE} configuration for ${CMAKE_SYSTEM_NAME} (${CMAKE_SYSTEM_PROCESSOR})</text>
PackageVerificationCode: \${SBOM_VERIFICATION_CODE}
BuiltDate: ${NOW_UTC}
Relationship: SPDXRef-DOCUMENT DESCRIBES SPDXRef-${SBOM_GENERATE_PROJECT}
"
		)

		install(
			CODE "
				file(READ \"${_f}\" _f_contents)
				file(APPEND \"${PROJECT_BINARY_DIR}/sbom/sbom.spdx.in\" \"\${_f_contents}\")
			"
		)

		set(SBOM_LAST_SPDXID
		    "SPDXRef-${SBOM_GENERATE_PROJECT}"
		    PARENT_SCOPE
		)
	else()
		foreach(_f IN LISTS SBOM_GENERATE_INPUT)
			get_filename_component(_f_name "${_f}" NAME)
			set(_f_in "${CMAKE_CURRENT_BINARY_DIR}/${_f_name}")
			set(_f_in_gen "${_f_in}_gen")
			configure_file("${_f}" "${_f_in}" @ONLY)
			file(
				GENERATE
				OUTPUT "${_f_in_gen}"
				INPUT "${_f_in}"
			)
			install(
				CODE "
					file(READ \"${_f_in_gen}\" _f_contents)
					file(APPEND \"${PROJECT_BINARY_DIR}/sbom/sbom.spdx.in\" \"\${_f_contents}\")
				"
			)
		endforeach()

		set(SBOM_LAST_SPDXID
		    ""
		    PARENT_SCOPE
		)
	endif()

	install(CODE "set(SBOM_VERIFICATION_CODES \"\")")

	set_property(GLOBAL PROPERTY SBOM_FILENAME "${SBOM_GENERATE_OUTPUT}")
	set(SBOM_FILENAME
	    "${SBOM_GENERATE_OUTPUT}"
	    PARENT_SCOPE
	)
	set_property(GLOBAL PROPERTY sbom_project "${SBOM_GENERATE_PROJECT}")
	set_property(GLOBAL PROPERTY sbom_packages "")
	set_property(GLOBAL PROPERTY sbom_licenses "")
	set_property(GLOBAL PROPERTY sbom_relations "")
	set_property(GLOBAL PROPERTY sbom_osv_file "${SBOM_GENERATE_OSV_QUERY}")
	set_property(GLOBAL PROPERTY sbom_osv "")

	file(WRITE ${PROJECT_BINARY_DIR}/sbom/CMakeLists.txt "")

	sbom_licence_try("${SBOM_GENERATE_LICENSE}")
endfunction()

# Find python.
#
# Usage sbom_find_python([REQUIRED])
macro(sbom_find_python)
	if(Python3_EXECUTABLE)
		set(Python3_FOUND TRUE)
	elseif(NOT CMAKE_VERSION VERSION_LESS 3.12)
		find_package(Python3 COMPONENTS Interpreter ${ARGV})
	else()
		if(WIN32)
			find_program(Python3_EXECUTABLE NAMES python ${ARGV})
		else()
			find_program(Python3_EXECUTABLE NAMES python3 ${ARGV})
		endif()

		if(Python3_EXECUTABLE)
			set(Python3_FOUND TRUE)
		else()
			set(Python3_FOUND FALSE)
		endif()
	endif()

	if(Python3_FOUND)
		if(NOT DEFINED SBOM_HAVE_PYTHON_DEPS)
			execute_process(
				COMMAND
					${Python3_EXECUTABLE} -c "
import reuse
import spdx_tools.spdx.clitools.pyspdxtools
import ntia_conformance_checker.main
"
				RESULT_VARIABLE _res
				ERROR_QUIET OUTPUT_QUIET
			)

			if("${_res}" STREQUAL "0")
				set(SBOM_HAVE_PYTHON_DEPS
				    TRUE
				    CACHE INTERNAL ""
				)
			else()
				set(SBOM_HAVE_PYTHON_DEPS
				    FALSE
				    CACHE INTERNAL ""
				)
			endif()
		endif()

		if("${ARGN}" STREQUAL "REQUIRED" AND NOT SBOM_HAVE_PYTHON_DEPS)
			message(FATAL_ERROR "Missing python packages")
		endif()
	endif()
endmacro()

# Verify the generated SBOM. Call after sbom_generate() and other SBOM populating commands.
function(sbom_finalize)
	set(options NO_VERIFY VERIFY)
	set(oneValueArgs GRAPH)
	set(multiValueArgs)
	cmake_parse_arguments(
		SBOM_FINALIZE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
	)
	if(SBOM_FINALIZE_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Unknown arguments: ${SBOM_FINALIZE_UNPARSED_ARGUMENTS}")
	endif()

	get_property(_sbom GLOBAL PROPERTY SBOM_FILENAME)
	get_property(_sbom_project GLOBAL PROPERTY sbom_project)

	if("${_sbom_project}" STREQUAL "")
		message(FATAL_ERROR "Call sbom_generate() first")
	endif()

	get_property(_packages GLOBAL PROPERTY sbom_packages)
	foreach(_p IN LISTS _packages)
		file(APPEND ${PROJECT_BINARY_DIR}/sbom/CMakeLists.txt "install(SCRIPT ${_p})
"
		)
	endforeach()

	get_property(_licenses GLOBAL PROPERTY sbom_licenses)
	foreach(_lic IN LISTS _licenses)
		file(APPEND ${PROJECT_BINARY_DIR}/sbom/CMakeLists.txt
		     "install(SCRIPT ${PROJECT_BINARY_DIR}/sbom/${_lic}.cmake)
"
		)
	endforeach()

	get_property(_relations GLOBAL PROPERTY sbom_relations)
	foreach(_rel IN LISTS _relations)
		file(APPEND ${PROJECT_BINARY_DIR}/sbom/CMakeLists.txt "install(SCRIPT ${_rel})
"
		)
	endforeach()

	file(
		WRITE ${PROJECT_BINARY_DIR}/sbom/verify.cmake
		"
		message(STATUS \"Finalizing: ${_sbom}\")
		list(SORT SBOM_VERIFICATION_CODES)
		string(REPLACE \";\" \"\" SBOM_VERIFICATION_CODES \"\${SBOM_VERIFICATION_CODES}\")
		file(WRITE \"${PROJECT_BINARY_DIR}/sbom/verification.txt\" \"\${SBOM_VERIFICATION_CODES}\")
		file(SHA1 \"${PROJECT_BINARY_DIR}/sbom/verification.txt\" SBOM_VERIFICATION_CODE)
		if(DEFINED ENV{DESTDIR})
			set(_dest \"\$ENV{DESTDIR}${_sbom}\")
		else()
			set(_dest \"${_sbom}\")
		endif()
		get_filename_component(_dest_dir \"\${_dest}\" DIRECTORY)
		file(MAKE_DIRECTORY \"\${_dest_dir}\")
		configure_file(\"${PROJECT_BINARY_DIR}/sbom/sbom.spdx.in\" \"\${_dest}\" COPYONLY)
		"
	)

	if(NOT "${SBOM_FINALIZE_GRAPH}" STREQUAL "")
		set(SBOM_FINALIZE_NO_VERIFY FALSE)
		set(SBOM_FINALIZE_VERIFY TRUE)
		set(_graph --graph --outfile "${SBOM_FINALIZE_GRAPH}")
	else()
		set(_graph)
	endif()

	if(SBOM_FINALIZE_NO_VERIFY)
		set(SBOM_FINALIZE_VERIFY FALSE)
	else()
		if(SBOM_FINALIZE_VERIFY)
			# Force verify.
			set(_req REQUIRED)
		else()
			# Check if we can verify.
			set(_req)
		endif()

		sbom_find_python(${_req})

		if(Python3_FOUND)
			set(SBOM_FINALIZE_VERIFY TRUE)
		endif()
	endif()

	if(SBOM_FINALIZE_VERIFY)
		file(
			APPEND ${PROJECT_BINARY_DIR}/sbom/verify.cmake
			"
			message(STATUS \"Verifying: ${_sbom}\")
			execute_process(
				COMMAND ${Python3_EXECUTABLE} -m spdx_tools.spdx.clitools.pyspdxtools
				-i \"${_sbom}\" ${_graph}
				RESULT_VARIABLE _res
			)
			if(NOT _res EQUAL 0)
				message(FATAL_ERROR \"SBOM verification failed\")
			endif()

			execute_process(
				COMMAND ${Python3_EXECUTABLE} -m ntia_conformance_checker.main
				--file \"${_sbom}\"
				RESULT_VARIABLE _res
			)
			if(NOT _res EQUAL 0)
				message(FATAL_ERROR \"SBOM NTIA verification failed\")
			endif()
			"
		)
	endif()

	file(APPEND ${PROJECT_BINARY_DIR}/sbom/CMakeLists.txt "install(SCRIPT verify.cmake)
"
	)

	# Workaround for pre-CMP0082.
	add_subdirectory(${PROJECT_BINARY_DIR}/sbom ${PROJECT_BINARY_DIR}/sbom)

	get_property(_osv GLOBAL PROPERTY sbom_osv)
	get_property(_osv_file GLOBAL PROPERTY sbom_osv_file)
	if(NOT "${_osv}" STREQUAL "" AND NOT "${_osv_file}" STREQUAL "")
		file(
			WRITE "${_osv_file}"
			"{
	\"queries\": [
${_osv}
	]
}
"
		)
	endif()

	# Mark finalized.
	set(SBOM_FILENAME
	    "${_sbom}"
	    PARENT_SCOPE
	)
	set_property(GLOBAL PROPERTY sbom_project "")
endfunction()

# Append a package to the OSV JSON output file.
function(osv_add)
	set(options)
	set(oneValueArgs PACKAGE VERSION COMMIT ECOSYSTEM PURL)
	set(multiValueArgs)
	cmake_parse_arguments(OSV_ADD "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	if(OSV_ADD_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Unknown arguments: ${OSV_ADD_UNPARSED_ARGUMENTS}")
	endif()

	if("${OSV_ADD_PACKAGE}" STREQUAL "")
		message(FATAL_ERROR "Missing PACKAGE argument")
	endif()

	if("${OSV_ADD_VERSION}" STREQUAL "" AND "${OSV_ADD_COMMIT}" STREQUAL "")
		message(FATAL_ERROR "Missing VERSION or COMMIT argument")
	endif()

	if("${OSV_ADD_ECOSYSTEM}" STREQUAL ""
	   AND "${OSV_ADD_PURL}" STREQUAL ""
	   AND "${OSV_ADD_COMMIT}" STREQUAL ""
	)
		message(FATAL_ERROR "Missing ECOSYSTEM or PURL argument")
	endif()

	set(query "		{
			"
	)

	if(NOT "${OSV_ADD_COMMIT}" STREQUAL "")
		set(query "${query}\"commit\": \"${OSV_ADD_COMMIT}\"")
	else()
		set(query "${query}\"version\": \"${OSV_ADD_VERSION}\"")
	endif()

	if("${OSV_ADD_COMMIT}" STREQUAL "" OR NOT "${OSV_ADD_PURL}" STREQUAL "")
		set(query
		    "${query},
			\"package\": {
				"
		)

		if(NOT "${OSV_ADD_PURL}" STREQUAL "")
			set(query "${query}\"purl\": \"${OSV_ADD_PURL}\"")
		else()
			set(query "${query}\"name\": \"${OSV_ADD_PACKAGE}\",
				\"ecosystem\": \"${OSV_ADD_ECOSYSTEM}\""
			)
		endif()

		set(query "${query}
			}"
		)
	endif()

	set(query "${query}
		}"
	)

	get_property(_osv GLOBAL PROPERTY sbom_osv)
	if("${_osv}" STREQUAL "")
		set(_osv "${query}")
	else()
		set(_osv "${_osv},
${query}"
		)
	endif()

	set_property(GLOBAL PROPERTY sbom_osv "${_osv}")
endfunction()

# Append a file to the SBOM. Use this after calling sbom_generate().
function(sbom_file)
	set(options OPTIONAL)
	set(oneValueArgs FILENAME FILETYPE LICENSE RELATIONSHIP SPDXID)
	set(multiValueArgs)
	cmake_parse_arguments(SBOM_FILE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	if(SBOM_FILE_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Unknown arguments: ${SBOM_FILE_UNPARSED_ARGUMENTS}")
	endif()

	if("${SBOM_FILE_FILENAME}" STREQUAL "")
		message(FATAL_ERROR "Missing FILENAME argument")
	endif()

	if("${SBOM_FILE_LICENSE}" STREQUAL "")
		set(SBOM_FILE_LICENSE "NOASSERTION")
	endif()

	sbom_spdxid(
		VARIABLE SBOM_FILE_SPDXID
		CHECK "${SBOM_FILE_SPDXID}"
		HINTS "SPDXRef-${SBOM_FILE_FILENAME}"
	)

	set(SBOM_LAST_SPDXID
	    "${SBOM_FILE_SPDXID}"
	    PARENT_SCOPE
	)

	if("${SBOM_FILE_FILETYPE}" STREQUAL "")
		message(FATAL_ERROR "Missing FILETYPE argument")
	endif()

	get_property(_sbom GLOBAL PROPERTY SBOM_FILENAME)
	get_property(_sbom_project GLOBAL PROPERTY sbom_project)

	set(relationship "")
	if(NOT "${SBOM_FILE_RELATIONSHIP}" STREQUAL "")
		string(REPLACE "@SBOM_LAST_SPDXID@" "${SBOM_FILE_SPDXID}" SBOM_FILE_RELATIONSHIP
			       "${SBOM_FILE_RELATIONSHIP}"
		)

		set(relationship "
Relationship: ${SBOM_FILE_RELATIONSHIP}"
		)
	endif()

	if("${_sbom_project}" STREQUAL "")
		message(FATAL_ERROR "Call sbom_generate() first")
	endif()

	sbom_licence_try("${SBOM_FILE_LICENSE}")

	file(
		GENERATE
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${SBOM_FILE_SPDXID}.cmake
		CONTENT
			"
			cmake_policy(SET CMP0011 NEW)
			cmake_policy(SET CMP0012 NEW)
			set(_install_root \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}\")
			if(NOT EXISTS \"\${_install_root}/${SBOM_FILE_FILENAME}\")
				if(NOT ${SBOM_FILE_OPTIONAL})
					message(FATAL_ERROR \"Cannot find ${SBOM_FILE_FILENAME}\")
				endif()
			else()
				file(SHA1 \${_install_root}/${SBOM_FILE_FILENAME} _sha1)
				list(APPEND SBOM_VERIFICATION_CODES \${_sha1})
				file(APPEND \"${PROJECT_BINARY_DIR}/sbom/sbom.spdx.in\"
\"
FileName: ./${SBOM_FILE_FILENAME}
SPDXID: ${SBOM_FILE_SPDXID}
FileType: ${SBOM_FILE_FILETYPE}
FileChecksum: SHA1: \${_sha1}
LicenseConcluded: ${SBOM_FILE_LICENSE}
LicenseInfoInFile: NOASSERTION
FileCopyrightText: NOASSERTION${relationship}
\"
				)
			endif()
			"
	)

	install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/${SBOM_FILE_SPDXID}.cmake)
endfunction()

# Append a target output to the SBOM. Use this after calling sbom_generate().
function(sbom_target)
	set(options)
	set(oneValueArgs TARGET)
	set(multiValueArgs)
	cmake_parse_arguments(
		SBOM_TARGET "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
	)

	if("${SBOM_TARGET_TARGET}" STREQUAL "")
		message(FATAL_ERROR "Missing TARGET argument")
	endif()

	get_target_property(_type ${SBOM_TARGET_TARGET} TYPE)
	if("${_type}" STREQUAL "EXECUTABLE")
        string(REPLACE "_" "-" target_id "${SBOM_TARGET_TARGET}")
        sbom_file(FILENAME ${CMAKE_INSTALL_BINDIR}/$<TARGET_FILE_NAME:${SBOM_TARGET_TARGET}>
                  FILETYPE BINARY ${SBOM_TARGET_UNPARSED_ARGUMENTS}
                  SPDXID       "SPDXRef-Binary-${target_id}"
        )
	elseif("${_type}" STREQUAL "STATIC_LIBRARY")
		sbom_file(FILENAME ${CMAKE_INSTALL_LIBDIR}/$<TARGET_FILE_NAME:${SBOM_TARGET_TARGET}>
			  FILETYPE BINARY ${SBOM_TARGET_UNPARSED_ARGUMENTS}
		)
	elseif("${_type}" STREQUAL "SHARED_LIBRARY")
		if(WIN32)
			sbom_file(
				FILENAME
					${CMAKE_INSTALL_BINDIR}/$<TARGET_FILE_NAME:${SBOM_TARGET_TARGET}>
				FILETYPE BINARY ${SBOM_TARGET_UNPARSED_ARGUMENTS}
			)
			sbom_file(
				FILENAME
					${CMAKE_INSTALL_LIBDIR}/$<TARGET_LINKER_FILE_NAME:${SBOM_TARGET_TARGET}>
				FILETYPE BINARY OPTIONAL ${SBOM_TARGET_UNPARSED_ARGUMENTS}
			)
		else()
			sbom_file(
				FILENAME
					${CMAKE_INSTALL_LIBDIR}/$<TARGET_FILE_NAME:${SBOM_TARGET_TARGET}>
				FILETYPE BINARY ${SBOM_TARGET_UNPARSED_ARGUMENTS}
			)
		endif()
	else()
		message(FATAL_ERROR "Unsupported target type ${_type}")
	endif()

	set(SBOM_LAST_SPDXID
	    "${SBOM_LAST_SPDXID}"
	    PARENT_SCOPE
	)
endfunction()

# Append all files recursively in a directory to the SBOM. Use this after calling sbom_generate().
function(sbom_directory)
	set(options)
	set(oneValueArgs DIRECTORY FILETYPE LICENSE RELATIONSHIP)
	set(multiValueArgs)
	cmake_parse_arguments(
		SBOM_DIRECTORY "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
	)
	if(SBOM_DIRECTORY_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Unknown arguments: ${SBOM_DIRECTORY_UNPARSED_ARGUMENTS}")
	endif()

	if("${SBOM_DIRECTORY_DIRECTORY}" STREQUAL "")
		message(FATAL_ERROR "Missing DIRECTORY argument")
	endif()

	sbom_spdxid(VARIABLE SBOM_DIRECTORY_SPDXID HINTS "SPDXRef-${SBOM_DIRECTORY_DIRECTORY}")

	set(SBOM_LAST_SPDXID "${SBOM_DIRECTORY_SPDXID}")

	if("${SBOM_DIRECTORY_FILETYPE}" STREQUAL "")
		message(FATAL_ERROR "Missing FILETYPE argument")
	endif()

	if("${SBOM_DIRECTORY_LICENSE}" STREQUAL "")
		set(SBOM_DIRECTORY_LICENSE "NOASSERTION")
	endif()

	get_property(_sbom GLOBAL PROPERTY SBOM_FILENAME)
	get_property(_sbom_project GLOBAL PROPERTY sbom_project)

	if("${SBOM_DIRECTORY_RELATIONSHIP}" STREQUAL "")
		set(SBOM_DIRECTORY_RELATIONSHIP
		    "SPDXRef-${_sbom_project} CONTAINS ${SBOM_DIRECTORY_SPDXID}"
		)
	else()
		string(REPLACE "@SBOM_LAST_SPDXID@" "${SBOM_DIRECTORY_SPDXID}"
			       SBOM_DIRECTORY_RELATIONSHIP "${SBOM_DIRECTORY_RELATIONSHIP}"
		)
	endif()

	if("${_sbom_project}" STREQUAL "")
		message(FATAL_ERROR "Call sbom_generate() first")
	endif()

	sbom_licence_try("${SBOM_DIRECTORY_LICENSE}")

	file(
		GENERATE
		OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${SBOM_DIRECTORY_SPDXID}.cmake"
		CONTENT
			"
			file(GLOB_RECURSE _files
				LIST_DIRECTORIES false RELATIVE \"\${CMAKE_INSTALL_PREFIX}\"
				\"\${CMAKE_INSTALL_PREFIX}/${SBOM_DIRECTORY_DIRECTORY}/*\"
			)

			set(_count 0)
			foreach(_f IN LISTS _files)
				file(SHA1 \"\${CMAKE_INSTALL_PREFIX}/\${_f}\" _sha1)
				list(APPEND SBOM_VERIFICATION_CODES \${_sha1})
				file(APPEND \"${PROJECT_BINARY_DIR}/sbom/sbom.spdx.in\"
\"
FileName: ./\${_f}
SPDXID: ${SBOM_DIRECTORY_SPDXID}-\${_count}
FileType: ${SBOM_DIRECTORY_FILETYPE}
FileChecksum: SHA1: \${_sha1}
LicenseConcluded: ${SBOM_DIRECTORY_LICENSE}
LicenseInfoInFile: NOASSERTION
FileCopyrightText: NOASSERTION
Relationship: ${SBOM_DIRECTORY_RELATIONSHIP}-\${_count}
\"
				)
				math(EXPR _count \"\${_count} + 1\")
			endforeach()
			"
	)

	install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/${SBOM_DIRECTORY_SPDXID}.cmake)

	set(SBOM_LAST_SPDXID
	    ""
	    PARENT_SCOPE
	)
endfunction()

# Append a package (without files) to the SBOM. Use this after calling sbom_generate().
function(sbom_package)
	set(options)
	set(oneValueArgs
	    PACKAGE
	    VERSION
	    COMMIT
	    LICENSE
	    DOWNLOAD_LOCATION
	    RELATIONSHIP
	    SPDXID
	    SUPPLIER
	)
	set(multiValueArgs EXTREF)
	cmake_parse_arguments(
		SBOM_PACKAGE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
	)
	if(SBOM_PACKAGE_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Unknown arguments: ${SBOM_PACKAGE_UNPARSED_ARGUMENTS}")
	endif()

	if("${SBOM_PACKAGE_PACKAGE}" STREQUAL "")
		message(FATAL_ERROR "Missing PACKAGE")
	endif()

	if("${SBOM_PACKAGE_DOWNLOAD_LOCATION}" STREQUAL "")
		set(SBOM_PACKAGE_DOWNLOAD_LOCATION NOASSERTION)
	endif()

	set(_purl "")
	foreach(_e IN LISTS SBOM_PACKAGE_EXTREF)
		if("${_e}" MATCHES "^PACKAGE-MANAGER purl ")
			string(REGEX REPLACE "^PACKAGE-MANAGER purl " "" _purl "${_e}")
			break()
		endif()
	endforeach()

	if(NOT "${_purl}" STREQUAL "" OR NOT "${SBOM_PACKAGE_COMMIT}" STREQUAL "")
		osv_add(
			PACKAGE ${SBOM_PACKAGE_PACKAGE}
			COMMIT ${SBOM_PACKAGE_COMMIT}
			VERSION ${SBOM_PACKAGE_VERSION}
			PURL "${_purl}"
		)
	endif()

	sbom_spdxid(
		VARIABLE SBOM_PACKAGE_SPDXID
		CHECK "${SBOM_PACKAGE_SPDXID}"
		HINTS "SPDXRef-${SBOM_PACKAGE_PACKAGE}"
	)

	set(SBOM_LAST_SPDXID
	    "${SBOM_PACKAGE_SPDXID}"
	    PARENT_SCOPE
	)

	set(_fields)

	if("${SBOM_PACKAGE_VERSION}" STREQUAL "")
		if(NOT "${SBOM_PACKAGE_COMMIT}" STREQUAL "")
			set(SBOM_PACKAGE_VERSION "${SBOM_PACKAGE_COMMIT}")
		else()
			set(SBOM_PACKAGE_VERSION "unknown")
		endif()
	endif()

	if(NOT "${SBOM_PACKAGE_COMMIT}" STREQUAL "")
		set(_fields "${_fields}
PackageSourceInfo: Commit:${SBOM_PACKAGE_COMMIT}"
		)
	endif()

	if("${SBOM_PACKAGE_SUPPLIER}" STREQUAL "")
		set(SBOM_PACKAGE_SUPPLIER "Person: Anonymous")
	endif()

	if(NOT "${SBOM_PACKAGE_LICENSE}" STREQUAL "")
		set(_fields "${_fields}
PackageLicenseConcluded: ${SBOM_PACKAGE_LICENSE}"
		)

		sbom_licence_try("${SBOM_PACKAGE_LICENSE}")
	else()
		set(_fields "${_fields}
PackageLicenseConcluded: NOASSERTION"
		)
	endif()

	foreach(_ref IN LISTS SBOM_PACKAGE_EXTREF)
		set(_fields "${_fields}
ExternalRef: ${_ref}"
		)
	endforeach()

	get_property(_sbom GLOBAL PROPERTY SBOM_FILENAME)
	get_property(_sbom_project GLOBAL PROPERTY sbom_project)

	if("${SBOM_PACKAGE_RELATIONSHIP}" STREQUAL "")
		set(SBOM_PACKAGE_RELATIONSHIP
		    "SPDXRef-${_sbom_project} DEPENDS_ON ${SBOM_PACKAGE_SPDXID}"
		)
	else()
		string(REPLACE "@SBOM_LAST_SPDXID@" "${SBOM_PACKAGE_SPDXID}"
			       SBOM_PACKAGE_RELATIONSHIP "${SBOM_PACKAGE_RELATIONSHIP}"
		)
	endif()

	if("${_sbom_project}" STREQUAL "")
		message(FATAL_ERROR "Call sbom_generate() first")
	endif()

	file(
		GENERATE
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${SBOM_PACKAGE_SPDXID}.cmake
		CONTENT
			"
			file(APPEND \"${PROJECT_BINARY_DIR}/sbom/sbom.spdx.in\"
\"
PackageName: ${SBOM_PACKAGE_PACKAGE}
SPDXID: ${SBOM_PACKAGE_SPDXID}
PackageDownloadLocation: ${SBOM_PACKAGE_DOWNLOAD_LOCATION}
PackageLicenseDeclared: NOASSERTION
PackageCopyrightText: NOASSERTION
PackageVersion: ${SBOM_PACKAGE_VERSION}
PackageSupplier: Organization: ${SBOM_PACKAGE_SUPPLIER}
FilesAnalyzed: false${_fields}
Relationship: ${SBOM_PACKAGE_RELATIONSHIP}
\"
			)
			"
	)

	get_property(_packages GLOBAL PROPERTY sbom_packages)
	list(APPEND _packages "${CMAKE_CURRENT_BINARY_DIR}/${SBOM_PACKAGE_SPDXID}.cmake")
	set_property(GLOBAL PROPERTY sbom_packages "${_packages}")
endfunction()

# Add a reference to a package in an external file.
function(sbom_external)
	set(options)
	set(oneValueArgs EXTERNAL FILENAME RENAME SPDXID RELATIONSHIP)
	set(multiValueArgs)
	cmake_parse_arguments(
		SBOM_EXTERNAL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
	)
	if(SBOM_EXTERNAL_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Unknown arguments: ${SBOM_EXTERNAL_UNPARSED_ARGUMENTS}")
	endif()

	if("${SBOM_EXTERNAL_EXTERNAL}" STREQUAL "")
		message(FATAL_ERROR "Missing EXTERNAL")
	endif()

	if("${SBOM_EXTERNAL_FILENAME}" STREQUAL "")
		message(FATAL_ERROR "Missing FILENAME")
	endif()

	if("${SBOM_EXTERNAL_SPDXID}" STREQUAL "")
		set(SBOM_EXTERNAL_SPDXID "DocumentRef-EXTERNAL-${SBOM_EXTERNAL_EXTERNAL}")
	endif()

	if(NOT "${SBOM_EXTERNAL_SPDXID}" MATCHES "^DocumentRef-[-a-zA-Z0-9]+$")
		message(FATAL_ERROR "Invalid DocumentRef \"${SBOM_EXTERNAL_SPDXID}\"")
	endif()

	set(SBOM_LAST_SPDXID
	    "${SBOM_EXTERNAL_SPDXID}"
	    PARENT_SCOPE
	)

	get_property(_sbom GLOBAL PROPERTY SBOM_FILENAME)
	get_property(_sbom_project GLOBAL PROPERTY sbom_project)

	if("${_sbom_project}" STREQUAL "")
		message(FATAL_ERROR "Call sbom_generate() first")
	endif()

	get_filename_component(sbom_dir "${_sbom}" DIRECTORY)

	if("${SBOM_EXTERNAL_RELATIONSHIP}" STREQUAL "")
		set(SBOM_EXTERNAL_RELATIONSHIP
		    "SPDXRef-${_sbom_project} DEPENDS_ON ${SBOM_EXTERNAL_SPDXID}:${SBOM_EXTERNAL_EXTERNAL}"
		)
	else()
		string(REPLACE "@SBOM_LAST_SPDXID@" "${SBOM_EXTERNAL_SPDXID}"
			       SBOM_EXTERNAL_RELATIONSHIP "${SBOM_EXTERNAL_RELATIONSHIP}"
		)
	endif()

	# Filename may not exist yet, and it could be a generator expression.
	file(
		GENERATE
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${SBOM_EXTERNAL_SPDXID}.cmake
		CONTENT
			"
			file(SHA1 \"${SBOM_EXTERNAL_FILENAME}\" ext_sha1)
			file(READ \"${SBOM_EXTERNAL_FILENAME}\" ext_content)
			if(\"${SBOM_EXTERNAL_RENAME}\" STREQUAL \"\")
				get_filename_component(ext_name \"${SBOM_EXTERNAL_FILENAME}\" NAME)
				file(WRITE \"${sbom_dir}/\${ext_name}\" \"\${ext_content}\")
			else()
				file(WRITE \"${sbom_dir}/${SBOM_EXTERNAL_RENAME}\" \"\${ext_content}\")
			endif()

			if(NOT \"\${ext_content}\" MATCHES \"[\\r\\n]DocumentNamespace:\")
				message(FATAL_ERROR \"Missing DocumentNamespace in ${SBOM_EXTERNAL_FILENAME}\")
			endif()

			string(REGEX REPLACE \"^.*[\\r\\n]DocumentNamespace:[ \\t]*([^#\\r\\n]*).*$\"
				\"\\\\1\" ext_ns \"\${ext_content}\")

			list(APPEND SBOM_EXT_DOCS \"
ExternalDocumentRef: ${SBOM_EXTERNAL_SPDXID} \${ext_ns} SHA1: \${ext_sha1}\")

			file(APPEND \"${PROJECT_BINARY_DIR}/sbom/sbom.spdx.in\"
\"
Relationship: ${SBOM_EXTERNAL_RELATIONSHIP}\")
		"
	)

	get_property(_relations GLOBAL PROPERTY sbom_relations)
	list(APPEND _relations "${CMAKE_CURRENT_BINARY_DIR}/${SBOM_EXTERNAL_SPDXID}.cmake")
	set_property(GLOBAL PROPERTY sbom_relations "${_relations}")
endfunction()

# Append a LicenseRef-... license to the SBOM.
function(sbom_license)
	set(options)
	set(oneValueArgs LICENSE NAME FILE TEXT)
	set(multiValueArgs)
	cmake_parse_arguments(
		SBOM_LICENSE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
	)
	if(SBOM_LICENSE_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Unknown arguments: ${SBOM_LICENSE_UNPARSED_ARGUMENTS}")
	endif()

	if("${SBOM_LICENSE_LICENSE}" STREQUAL "")
		message(FATAL_ERROR "Missing LICENSE")
	endif()

	if(NOT "${SBOM_LICENSE_LICENSE}" MATCHES "^LicenseRef-")
		message(FATAL_ERROR "Only LicenseRef-... licenses are supported")
	endif()

	if("${SBOM_LICENSE_NAME}" STREQUAL "")
		set(SBOM_LICENSE_NAME "NOASSERTION")
	endif()

	if("${SBOM_LICENSE_FILE}" STREQUAL "" AND "${SBOM_LICENSE_TEXT}" STREQUAL "")
		set(SBOM_LICENSE_FILE "${PROJECT_SOURCE_DIR}/LICENSES/${SBOM_LICENSE_LICENSE}.txt")
	endif()

	if("${SBOM_LICENSE_TEXT}" STREQUAL "")
		if(NOT EXISTS "${SBOM_LICENSE_FILE}")
			message(FATAL_ERROR "Cannot find ${SBOM_LICENSE_FILE}")
		endif()

		file(READ "${SBOM_LICENSE_FILE}" SBOM_LICENSE_TEXT)
		string(REGEX REPLACE "\r\n" "\n" SBOM_LICENSE_TEXT "${SBOM_LICENSE_TEXT}")
	endif()

	if("${SBOM_LICENSE_TEXT}" STREQUAL "")
		message(FATAL_ERROR "Empty license text")
	endif()

	get_property(_licenses GLOBAL PROPERTY sbom_licenses)
	if("${SBOM_LICENSE_LICENSE}" IN_LIST _licenses)
		message(FATAL_ERROR "License already added")
	endif()

	file(
		GENERATE
		OUTPUT ${PROJECT_BINARY_DIR}/sbom/${SBOM_LICENSE_LICENSE}.cmake
		CONTENT
			"
			file(APPEND \"${PROJECT_BINARY_DIR}/sbom/sbom.spdx.in\"
\"
LicenseID: ${SBOM_LICENSE_LICENSE}
LicenseName: ${SBOM_LICENSE_NAME}
ExtractedText: <text>\"
			)

			file(APPEND \"${PROJECT_BINARY_DIR}/sbom/sbom.spdx.in\"
				[=[${SBOM_LICENSE_TEXT}]=]
			)

			file(APPEND \"${PROJECT_BINARY_DIR}/sbom/sbom.spdx.in\"
\"</text>
\")
			"
	)

	list(APPEND _licenses "${SBOM_LICENSE_LICENSE}")
	set_property(GLOBAL PROPERTY sbom_licenses "${_licenses}")
endfunction()

# Try to add a license to the SBOM.
function(sbom_licence_try id)
	if(NOT "${id}" MATCHES "^LicenseRef-")
		# Not an external license.
		return()
	endif()

	get_property(_licenses GLOBAL PROPERTY sbom_licenses)
	if("${id}" IN_LIST _licenses)
		# Already included.
		return()
	endif()

	set(_file "${PROJECT_SOURCE_DIR}/LICENSES/${id}.txt")
	if(NOT EXISTS "${_file}")
		# We don't have the license text.
		return()
	endif()

	# Looks ok.
	sbom_license(LICENSE "${id}" FILE "${_file}")
endfunction()

# Append something to the SBOM. Use this after calling sbom_generate().
function(sbom_add type)
	if("${type}" STREQUAL "FILENAME")
		sbom_file(${ARGV})
	elseif("${type}" STREQUAL "DIRECTORY")
		sbom_directory(${ARGV})
	elseif("${type}" STREQUAL "TARGET")
		sbom_target(${ARGV})
	elseif("${type}" STREQUAL "PACKAGE")
		sbom_package(${ARGV})
	elseif("${type}" STREQUAL "EXTERNAL")
		sbom_external(${ARGV})
	elseif("${type}" STREQUAL "LICENSE")
		sbom_license(${ARGV})
	else()
		message(FATAL_ERROR "Unsupported sbom_add(${type})")
	endif()

	set(SBOM_LAST_SPDXID
	    "${SBOM_LAST_SPDXID}"
	    PARENT_SCOPE
	)
endfunction()

# Adds a target that performs `python3 -m reuse lint'.  Python is required with the proper packages
# installed (see dist/common/requirements.txt).
function(reuse_lint)
	set(options CONFIG ALL)
	set(oneValueArgs TARGET)
	set(multiValueArgs)
	cmake_parse_arguments(REUSE_LINT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(NOT REUSE_LINT_TARGET)
		set(REUSE_LINT_TARGET ${PROJECT_NAME}-reuse-lint)
	endif()

	if(REUSE_LINT_ALL OR NOT REUSE_LINT_CONFIG)
		set(lint_all ALL)
	else()
		set(lint_all)
	endif()

	if(NOT TARGET ${REUSE_LINT_TARGET})
		sbom_find_python(REQUIRED)

		add_custom_target(
			${REUSE_LINT_TARGET}
			${lint_all}
			COMMAND ${Python3_EXECUTABLE} -m reuse --root "${PROJECT_SOURCE_DIR}" lint
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			VERBATIM
		)
	endif()

	if(REUSE_LINT_CONFIG)
		sbom_find_python(REQUIRED)

		# It seems that there is a race in linting and generating build artifacts. So, run
		# this (also) during config, to make sure that there is nothing else going on.
		execute_process(
			COMMAND ${Python3_EXECUTABLE} -m reuse --root "${PROJECT_SOURCE_DIR}" lint
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			RESULT_VARIABLE res
		)

		if(NOT "${res}" EQUAL 0)
			message(FATAL_ERROR "${REUSE_LINT_TARGET} failed")
		endif()
	endif()
endfunction()

# Adds a target that generates a SPDX file of the source code.  Python is required with the proper
# packages installed (see dist/common/requirements.txt).
function(reuse_spdx)
	set(options)
	set(oneValueArgs TARGET OUTPUT)
	set(multiValueArgs)
	cmake_parse_arguments(REUSE_SPDX "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(NOT REUSE_SPDX_TARGET)
		set(REUSE_SPDX_TARGET ${PROJECT_NAME}-reuse-spdx)
	endif()

	if(NOT REUSE_SPDX_OUTPUT)
		set(REUSE_SPDX_OUTPUT "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-src.spdx")
	endif()

	if(NOT TARGET ${REUSE_SPDX_TARGET})
		sbom_find_python(REQUIRED)

		add_custom_target(
			${REUSE_SPDX_TARGET} ALL
			COMMAND ${Python3_EXECUTABLE} -m reuse --root "${PROJECT_SOURCE_DIR}" spdx
				-o "${REUSE_SPDX_OUTPUT}"
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			VERBATIM
		)
	endif()
endfunction()