/*
 *  Copyright (C) 2011, 2012, 2013 Stephen F. Booth <me@sbooth.org>
 *  All Rights Reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    - Neither the name of Stephen F. Booth nor the names of its 
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <memory>

#include <taglib/tfilestream.h>
#include <taglib/speexfile.h>

#include "OggSpeexMetadata.h"
#include "CFErrorUtilities.h"
#include "AddXiphCommentToDictionary.h"
#include "SetXiphCommentFromMetadata.h"
#include "AddAudioPropertiesToDictionary.h"

#pragma mark Static Methods

CFArrayRef OggSpeexMetadata::CreateSupportedFileExtensions()
{
	CFStringRef supportedExtensions [] = { CFSTR("spx") };
	return CFArrayCreate(kCFAllocatorDefault, reinterpret_cast<const void **>(supportedExtensions), 1, &kCFTypeArrayCallBacks);
}

CFArrayRef OggSpeexMetadata::CreateSupportedMIMETypes()
{
	CFStringRef supportedMIMETypes [] = { CFSTR("audio/speex") };
	return CFArrayCreate(kCFAllocatorDefault, reinterpret_cast<const void **>(supportedMIMETypes), 1, &kCFTypeArrayCallBacks);
}

bool OggSpeexMetadata::HandlesFilesWithExtension(CFStringRef extension)
{
	if(nullptr == extension)
		return false;
	
	if(kCFCompareEqualTo == CFStringCompare(extension, CFSTR("spx"), kCFCompareCaseInsensitive))
		return true;
	
	return false;
}

bool OggSpeexMetadata::HandlesMIMEType(CFStringRef mimeType)
{
	if(nullptr == mimeType)
		return false;
	
	if(kCFCompareEqualTo == CFStringCompare(mimeType, CFSTR("audio/speex"), kCFCompareCaseInsensitive))
		return true;
	
	return false;
}

#pragma mark Creation and Destruction

OggSpeexMetadata::OggSpeexMetadata(CFURLRef url)
	: AudioMetadata(url)
{}

OggSpeexMetadata::~OggSpeexMetadata()
{}

#pragma mark Functionality

bool OggSpeexMetadata::ReadMetadata(CFErrorRef *error)
{
	// Start from scratch
	CFDictionaryRemoveAllValues(mMetadata);
	CFDictionaryRemoveAllValues(mChangedMetadata);
	
	UInt8 buf [PATH_MAX];
	if(!CFURLGetFileSystemRepresentation(mURL, false, buf, PATH_MAX))
		return false;
	
	// TODO: Use unique_ptr once the switch to C++11 STL is made
	std::auto_ptr<TagLib::FileStream> stream(new TagLib::FileStream(reinterpret_cast<const char *>(buf), true));
	if(!stream->isOpen()) {
		if(error) {
			CFStringRef description = CFCopyLocalizedString(CFSTR("The file “%@” could not be opened for reading."), "");
			CFStringRef failureReason = CFCopyLocalizedString(CFSTR("Input/output error"), "");
			CFStringRef recoverySuggestion = CFCopyLocalizedString(CFSTR("The file may have been renamed, moved, deleted, or you may not have appropriate permissions."), "");

			*error = CreateErrorForURL(AudioMetadataErrorDomain, AudioMetadataInputOutputError, description, mURL, failureReason, recoverySuggestion);

			CFRelease(description), description = nullptr;
			CFRelease(failureReason), failureReason = nullptr;
			CFRelease(recoverySuggestion), recoverySuggestion = nullptr;
		}

		return false;
	}

	TagLib::Ogg::Speex::File file(stream.get());
	if(!file.isValid()) {
		if(nullptr != error) {
			CFStringRef description = CFCopyLocalizedString(CFSTR("The file “%@” is not a valid Ogg Speex file."), "");
			CFStringRef failureReason = CFCopyLocalizedString(CFSTR("Not an Ogg Speex file"), "");
			CFStringRef recoverySuggestion = CFCopyLocalizedString(CFSTR("The file's extension may not match the file's type."), "");
			
			*error = CreateErrorForURL(AudioMetadataErrorDomain, AudioMetadataInputOutputError, description, mURL, failureReason, recoverySuggestion);
			
			CFRelease(description), description = nullptr;
			CFRelease(failureReason), failureReason = nullptr;
			CFRelease(recoverySuggestion), recoverySuggestion = nullptr;
		}
		
		return false;
	}
	
	CFDictionarySetValue(mMetadata, kPropertiesFormatNameKey, CFSTR("Ogg Speex"));
	
	if(file.audioProperties())
		AddAudioPropertiesToDictionary(mMetadata, file.audioProperties());
	
	if(file.tag()) {
		std::vector<AttachedPicture *> pictures;
		AddXiphCommentToDictionary(mMetadata, pictures, file.tag());
		for(auto picture : pictures)
			AddSavedPicture(picture);
	}
	
	return true;
}

bool OggSpeexMetadata::WriteMetadata(CFErrorRef *error)
{
	UInt8 buf [PATH_MAX];
	if(!CFURLGetFileSystemRepresentation(mURL, false, buf, PATH_MAX))
		return false;
	
	// TODO: Use unique_ptr once the switch to C++11 STL is made
	std::auto_ptr<TagLib::FileStream> stream(new TagLib::FileStream(reinterpret_cast<const char *>(buf)));
	if(!stream->isOpen()) {
		if(error) {
			CFStringRef description = CFCopyLocalizedString(CFSTR("The file “%@” could not be opened for writing."), "");
			CFStringRef failureReason = CFCopyLocalizedString(CFSTR("Input/output error"), "");
			CFStringRef recoverySuggestion = CFCopyLocalizedString(CFSTR("The file may have been renamed, moved, deleted, or you may not have appropriate permissions."), "");

			*error = CreateErrorForURL(AudioMetadataErrorDomain, AudioMetadataInputOutputError, description, mURL, failureReason, recoverySuggestion);

			CFRelease(description), description = nullptr;
			CFRelease(failureReason), failureReason = nullptr;
			CFRelease(recoverySuggestion), recoverySuggestion = nullptr;
		}

		return false;
	}

	TagLib::Ogg::Speex::File file(stream.get(), false);
	if(!file.isValid()) {
		if(error) {
			CFStringRef description = CFCopyLocalizedString(CFSTR("The file “%@” is not a valid Ogg Speex file."), "");
			CFStringRef failureReason = CFCopyLocalizedString(CFSTR("Not an Ogg Speex file"), "");
			CFStringRef recoverySuggestion = CFCopyLocalizedString(CFSTR("The file's extension may not match the file's type."), "");
			
			*error = CreateErrorForURL(AudioMetadataErrorDomain, AudioMetadataInputOutputError, description, mURL, failureReason, recoverySuggestion);
			
			CFRelease(description), description = nullptr;
			CFRelease(failureReason), failureReason = nullptr;
			CFRelease(recoverySuggestion), recoverySuggestion = nullptr;
		}
		
		return false;
	}
	
	SetXiphCommentFromMetadata(*this, file.tag());
	
	if(!file.save()) {
		if(error) {
			CFStringRef description = CFCopyLocalizedString(CFSTR("The file “%@” is not a valid Ogg Speex file."), "");
			CFStringRef failureReason = CFCopyLocalizedString(CFSTR("Unable to write metadata"), "");
			CFStringRef recoverySuggestion = CFCopyLocalizedString(CFSTR("The file's extension may not match the file's type."), "");
			
			*error = CreateErrorForURL(AudioMetadataErrorDomain, AudioMetadataInputOutputError, description, mURL, failureReason, recoverySuggestion);
			
			CFRelease(description), description = nullptr;
			CFRelease(failureReason), failureReason = nullptr;
			CFRelease(recoverySuggestion), recoverySuggestion = nullptr;
		}
		
		return false;
	}
	
	MergeChangedMetadataIntoMetadata();
	
	return true;
}
