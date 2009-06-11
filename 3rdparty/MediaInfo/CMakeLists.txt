project(mediainfo)

set(MEDIAINFO_MAJOR_VERSION 0)
set(MEDIAINFO_MINOR_VERSION 7)
set(MEDIAINFO_PATCH_VERSION 9)
set(MEDIAINFO_VERSION ${MEDIAINFO_MAJOR_VERSION}.${MEDIAINFO_MINOR_VERSION}.${MEDIAINFO_PATCH_VERSION})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

include_directories(${ZLIB_INCLUDE_DIR})

add_definitions(-DMEDIAINFO_MINIMIZESIZE)
if (NOT MINGW)
	# MinGW STL does not support wide characters (wchar_t)
	# see http://www.mingw.org/wiki/wide_characters
	add_definitions(-DUNICODE)
endif (NOT MINGW)

set(mediainfo_SRCS
	File__Analyze.cpp
	File__Analyze_Buffer.cpp
	File__Analyze_Buffer_MinimizeSize.cpp
	File__Analyze_Finalize.cpp
	File__Analyze_Streams.cpp
	File__Base.cpp
	File__MultipleParsing.cpp
	File__Duplicate.cpp
	File_Dummy.cpp
	File_Other.cpp
	File_Unknown.cpp
	MediaInfo.cpp
	MediaInfo_Config.cpp
	MediaInfo_Config_Automatic.cpp
	MediaInfo_Config_MediaInfo.cpp
	MediaInfo_File.cpp
	MediaInfo_Inform.cpp
	MediaInfo_Internal.cpp
	MediaInfoList.cpp
	MediaInfoList_Internal.cpp
	Archive/File_7z.cpp
	Archive/File_Ace.cpp
	Archive/File_Bzip2.cpp
	Archive/File_Elf.cpp
	Archive/File_Gzip.cpp
	Archive/File_Mz.cpp
	Archive/File_Rar.cpp
	Archive/File_Tar.cpp
	Archive/File_Zip.cpp
	Audio/File_Aac.cpp
	Audio/File_Ac3.cpp
	Audio/File_Adif.cpp
	Audio/File_Adpcm.cpp
	Audio/File_Adts.cpp
	Audio/File_Aes3.cpp
	Audio/File_Amr.cpp
	Audio/File_Amv.cpp
	Audio/File_Ape.cpp
	Audio/File_Au.cpp
	Audio/File_Dts.cpp
	Audio/File_ExtendedModule.cpp
	Audio/File_Flac.cpp
	Audio/File_ImpulseTracker.cpp
	Audio/File_Latm.cpp
	Audio/File_Midi.cpp
	Audio/File_Module.cpp
	Audio/File_Mpc.cpp
	Audio/File_MpcSv8.cpp
	Audio/File_Mpeg4_AudioSpecificConfig.cpp
	Audio/File_Mpega.cpp
	Audio/File_Pcm.cpp
	Audio/File_Ps2Audio.cpp
	Audio/File_ScreamTracker3.cpp
	Audio/File_Speex.cpp
	Audio/File_Tta.cpp
	Audio/File_TwinVQ.cpp
	Audio/File_Vorbis.cpp
	Audio/File_Wvpk.cpp
	Duplicate/File__Duplicate__Base.cpp
	Duplicate/File__Duplicate__Writer.cpp
	Duplicate/File__Duplicate_MpegTs.cpp
	Image/File_Bmp.cpp
	Image/File_Gif.cpp
	Image/File_Ico.cpp
	Image/File_Jpeg.cpp
	Image/File_Png.cpp
	Image/File_Rle.cpp
	Image/File_Tiff.cpp
	Multiple/File_Cdxa.cpp
	Multiple/File_Dpg.cpp
	Multiple/File_DvDif.cpp
	Multiple/File_Dvdv.cpp
	Multiple/File_Flv.cpp
	Multiple/File_Mk.cpp
	Multiple/File_Mpeg4.cpp
	Multiple/File_Mpeg4_Descriptors.cpp
	Multiple/File_Mpeg4_Elements.cpp
	Multiple/File_Mpeg4_TimeCode.cpp
	Multiple/File_Mpeg_Descriptors.cpp
	Multiple/File_Mpeg_Psi.cpp
	Multiple/File_MpegPs.cpp
	Multiple/File_MpegTs.cpp
	Multiple/File_MpegTs_Duplicate.cpp
	Multiple/File_Mxf.cpp
	Multiple/File_Nut.cpp
	Multiple/File_Ogg.cpp
	Multiple/File_Ogg_SubElement.cpp
	Multiple/File_Riff.cpp
	Multiple/File_Riff_Elements.cpp
	Multiple/File_Rm.cpp
	Multiple/File_Skm.cpp
	Multiple/File_Swf.cpp
	Multiple/File_Wm.cpp
	Multiple/File_Wm_Elements.cpp
	Tag/File__Tags.cpp
	Tag/File_ApeTag.cpp
	Tag/File_Id3.cpp
	Tag/File_Id3v2.cpp
	Tag/File_Lyrics3.cpp
	Tag/File_Lyrics3v2.cpp
	Tag/File_VorbisCom.cpp
	Text/File_Cmml.cpp
	Text/File_Kate.cpp
	Text/File_Eia608.cpp
	Text/File_Eia708.cpp
	Text/File_OtherText.cpp
	Text/File_Pgs.cpp
	Video/File_Avc.cpp
	Video/File_Avc_Duplicate.cpp
	Video/File_AvsV.cpp
	Video/File_Dirac.cpp
	Video/File_Flic.cpp
	Video/File_Mpeg4v.cpp
	Video/File_Mpegv.cpp
	Video/File_Theora.cpp
	Video/File_Vc1.cpp
	MediaInfoDLL.cpp
	MediaInfoDLL.def
)

if (WIN32)
	# FIXME MediaInfo.rc should use ${MEDIAINFO_MAJOR_VERSION} and friends
	list(APPEND mediainfo_SRCS
		MediaInfo.rc
	)
endif (WIN32)

add_library(mediainfo SHARED ${mediainfo_SRCS})

target_link_libraries(mediainfo
	zenlib
	${ZLIB_LIBRARY}
)

set_target_properties(mediainfo PROPERTIES
	VERSION ${MEDIAINFO_VERSION}
	SOVERSION ${MEDIAINFO_MAJOR_VERSION}
)

install(TARGETS mediainfo ${INSTALL_TARGETS_DEFAULT_ARGS})
