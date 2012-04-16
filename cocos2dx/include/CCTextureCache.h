/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __CCTEXTURE_CACHE_H__
#define __CCTEXTURE_CACHE_H__

#include <string>
#include "CCObject.h"
#include "CCDictionary.h"
#include "CCTexture2D.h"


#if CC_ENABLE_CACHE_TEXTTURE_DATA
    #include "CCImage.h"
    #include <list>
#endif

namespace   cocos2d {
class CCLock;
class CCImage;

/** @brief Singleton that handles the loading of textures
* Once the texture is loaded, the next time it will return
* a reference of the previously loaded texture reducing GPU & CPU memory
*/
class CC_DLL CCTextureCache : public CCObject
{
protected:
	CCDictionary* m_pTextures;
	//pthread_mutex_t				*m_pDictLock;


private:
	// @todo void addImageWithAsyncObject(CCAsyncObject* async);
    void addImageAsyncCallBack(ccTime dt);

public:

	CCTextureCache();
	virtual ~CCTextureCache();

	const char* description(void);

	/** Retruns ths shared instance of the cache */
	static CCTextureCache * sharedTextureCache();

	/** purges the cache. It releases the retained instance.
	@since v0.99.0
	*/
	static void purgeSharedTextureCache();

	/** Returns a Texture2D object given an file image
	* If the file image was not previously loaded, it will create a new CCTexture2D
	*  object and it will return it. It will use the filename as a key.
	* Otherwise it will return a reference of a previosly loaded image.
	* Supported image extensions: .png, .bmp, .tiff, .jpeg, .pvr, .gif
	*/
	CCTexture2D* addImage(const char* fileimage);

	/* Returns a Texture2D object given a file image
	* If the file image was not previously loaded, it will create a new CCTexture2D object and it will return it.
	* Otherwise it will load a texture in a new thread, and when the image is loaded, the callback will be called with the Texture2D as a parameter.
	* The callback will be called from the main thread, so it is safe to create any cocos2d object from the callback.
	* Supported image extensions: .png, .jpg
	* @since v0.8
	*/
	
	void addImageAsync(const char *path, CCObject *target, SEL_CallFuncO selector);

	/* Returns a Texture2D object given an CGImageRef image
	* If the image was not previously loaded, it will create a new CCTexture2D object and it will return it.
	* Otherwise it will return a reference of a previously loaded image
	* The "key" parameter will be used as the "key" for the cache.
	* If "key" is nil, then a new texture will be created each time.
	* @since v0.8
	*/
	// @todo CGImageRef CCTexture2D* addCGImage(CGImageRef image, string &  key);
	/** Returns a Texture2D object given an UIImage image
	* If the image was not previously loaded, it will create a new CCTexture2D object and it will return it.
	* Otherwise it will return a reference of a previously loaded image
	* The "key" parameter will be used as the "key" for the cache.
	* If "key" is nil, then a new texture will be created each time.
	*/
	CCTexture2D* addUIImage(CCImage *image, const char *key);

	/** Returns an already created texture. Returns nil if the texture doesn't exist.
	@since v0.99.5
	*/
	CCTexture2D* textureForKey(const char* key);
	/** Purges the dictionary of loaded textures.
	* Call this method if you receive the "Memory Warning"
	* In the short term: it will free some resources preventing your app from being killed
	* In the medium term: it will allocate more resources
	* In the long term: it will be the same
	*/
	void removeAllTextures();

	/** Removes unused textures
	* Textures that have a retain count of 1 will be deleted
	* It is convinient to call this method after when starting a new Scene
	* @since v0.8
	*/
	void removeUnusedTextures();

	/** Deletes a texture from the cache given a texture
	*/
	void removeTexture(CCTexture2D* texture);

	/** Deletes a texture from the cache given a its key name
	@since v0.99.4
	*/
	void removeTextureForKey(const char *textureKeyName);

	/** Output to CCLOG the current contents of this CCTextureCache
	* This will attempt to calculate the size of each texture, and the total texture memory in use
	*
	* @since v1.0
	*/
	void dumpCachedTextureInfo();

#ifdef CC_SUPPORT_PVRTC
	/** Returns a Texture2D object given an PVRTC RAW filename
	* If the file image was not previously loaded, it will create a new CCTexture2D
	*  object and it will return it. Otherwise it will return a reference of a previosly loaded image
	*
	* It can only load square images: width == height, and it must be a power of 2 (128,256,512...)
	* bpp can only be 2 or 4. 2 means more compression but lower quality.
	* hasAlpha: whether or not the image contains alpha channel
	*/
	CCTexture2D* addPVRTCImage(const char* fileimage, int bpp, bool hasAlpha, int width);
#endif // CC_SUPPORT_PVRTC
    
	/** Returns a Texture2D object given an PVR filename
	* If the file image was not previously loaded, it will create a new CCTexture2D
	*  object and it will return it. Otherwise it will return a reference of a previosly loaded image
	*/
	CCTexture2D* addPVRImage(const char* filename);

    /** Reload all textures
    It's only useful when the value of CC_ENABLE_CACHE_TEXTTURE_DATA is 1
    */
    static void reloadAllTextures();
};

#if CC_ENABLE_CACHE_TEXTTURE_DATA

class VolatileTexture
{
typedef enum {
	kInvalid = 0,
	kImageFile,
	kImageData,
	kString,
}ccCachedImageType;

public:
    VolatileTexture(CCTexture2D *t);
    ~VolatileTexture();

    static void addImageTexture(CCTexture2D *tt, const char* imageFileName, CCImage::EImageFormat format);
    static void addStringTexture(CCTexture2D *tt, const char* text, const CCSize& dimensions, CCTextAlignment alignment, const char *fontName, float fontSize);
	static void addDataTexture(CCTexture2D *tt, void* data, CCTexture2DPixelFormat pixelFormat, const CCSize& contentSize);

    static void removeTexture(CCTexture2D *t);
    static void reloadAllTextures();

public:
    static std::list<VolatileTexture*> textures;
    static bool isReloading;

protected:
    CCTexture2D *texture;

	ccCachedImageType m_eCashedImageType;

	void *m_pTextureData;
	CCSize m_TextureSize;
	CCTexture2DPixelFormat m_PixelFormat;

    std::string m_strFileName;
    CCImage::EImageFormat m_FmtImage;

    CCSize          m_size;
    CCTextAlignment m_alignment;
    std::string     m_strFontName;
    std::string     m_strText;
    float           m_fFontSize;
};

#endif

}//namespace   cocos2d 

#endif //__CCTEXTURE_CACHE_H__

