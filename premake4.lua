solution "taikod"
   
   configurations { "Debug", "Release" }

   platforms { "native", "x32", "x64" }

   location("build")

   configuration "Debug"
      targetdir "bin/debug"

   configuration "Release"
      targetdir "bin/release"

   project "taikod" 
      
      language "C++"

      includedirs { "external/libopkele/include", "/usr/local/Cellar/libconfuse/2.7/include" }
      libdirs { "/usr/local/Cellar/libconfuse/2.7/lib" }

      files { "src/**.h", "src/**.cpp", "src/**.c" }

      links { 
         "sqlite3", "curl", "expat", "ssl", "crypto", "z", "tidy", "libopkele",
         "confuse"
      }

      kind "ConsoleApp"

      configuration { "Debug" }
         defines { "DEBUG" } 
         flags { "Symbols" }

      configuration { "Release" }
         defines {}
         flags { "Optimize" }

      configuration "macosx" 
         links { "pam" }

      configuration "linux" 
         links { "dl", "uuid", "crypt" }

   project "libopkele"

      language "C++"

      includedirs { "external/libopkele/include" } 

      files { 
         "external/libopkele/lib/**.cc",
         "external/libopkele/include/**.h" 
      }

      kind "StaticLib"

      configuration { "Debug" } 
         flags { "Symbols" } 

      configuration { "Release" } 
         flags { "Optimize" } 
