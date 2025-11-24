# App Icon Setup

## Adding a Custom Icon

To add a custom icon to your PixL game:

### For macOS (.icns format)

1. **Create your icon image** (1024x1024 PNG recommended)
   - Design a vibrant, eye-catching icon for your fighting game
   - Use bold colors and clear imagery that represents PixL

2. **Convert to .icns format**
   
   **Option A: Using online converter**
   - Visit https://cloudconvert.com/png-to-icns
   - Upload your 1024x1024 PNG
   - Download the .icns file

   **Option B: Using macOS command line**
   ```bash
   # Create iconset directory
   mkdir AppIcon.iconset
   
   # Generate all required sizes (requires your source image as icon_1024x1024.png)
   sips -z 16 16     icon_1024x1024.png --out AppIcon.iconset/icon_16x16.png
   sips -z 32 32     icon_1024x1024.png --out AppIcon.iconset/icon_16x16@2x.png
   sips -z 32 32     icon_1024x1024.png --out AppIcon.iconset/icon_32x32.png
   sips -z 64 64     icon_1024x1024.png --out AppIcon.iconset/icon_32x32@2x.png
   sips -z 128 128   icon_1024x1024.png --out AppIcon.iconset/icon_128x128.png
   sips -z 256 256   icon_1024x1024.png --out AppIcon.iconset/icon_128x128@2x.png
   sips -z 256 256   icon_1024x1024.png --out AppIcon.iconset/icon_256x256.png
   sips -z 512 512   icon_1024x1024.png --out AppIcon.iconset/icon_256x256@2x.png
   sips -z 512 512   icon_1024x1024.png --out AppIcon.iconset/icon_512x512.png
   sips -z 1024 1024 icon_1024x1024.png --out AppIcon.iconset/icon_512x512@2x.png
   
   # Convert to .icns
   iconutil -c icns AppIcon.iconset
   ```

3. **Place the icon file**
   ```bash
   mv AppIcon.icns assets/Icon/
   ```

4. **Rebuild the project**
   ```bash
   rm -rf build
   cmake -B build -S .
   cmake --build build
   ```

5. **Your app bundle will now have the icon!**
   - The app will be located at `build/PixL.app`
   - You can drag it to your Applications folder
   - The icon will appear in Finder and the Dock

### Icon Design Tips

- **Size**: Start with at least 1024x1024 pixels
- **Format**: PNG with transparency works best
- **Style**: Bold, recognizable design that works at small sizes
- **Colors**: Vibrant colors that stand out in the dock
- **Theme**: Represent your fighting game - consider:
  - Silhouettes of fighters
  - Energy effects or combat symbols
  - Bold typography with "PixL"
  - Pixel art aesthetic

### Current Status

✅ CMake is configured to use `assets/Icon/AppIcon.icns`  
✅ App bundle settings are configured  
⏳ Waiting for you to add your custom icon file

Once you add the icon file and rebuild, your game will have a professional appearance in macOS!
