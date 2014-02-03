solution "openidd"
   
   configurations { "Debug", "Release" }

   platforms { "native", "x32", "x64" }

   location("build")

   configuration "Debug"
      targetdir "bin/debug"

   configuration "Release"
      targetdir "bin/release"

   project "openidd" 
      
      language "C++"

      includedirs { "external/libopkele/include", "/usr/local/Cellar/libconfuse/2.7/include" }
      libdirs { "/usr/local/Cellar/libconfuse/2.7/lib" }

      files { "src/**.h", "src/**.cpp", "src/**.c" }

      links { 
         "sqlite3", "curl", "expat", "ssl", "crypto", "z", "tidy", "pam", "libopkele",
         "confuse"
      }

      kind "ConsoleApp"

      configuration { "Debug" }
         defines { "DEBUG" } 
         flags { "Symbols" }

      configuration { "Release" }
         defines {}
         flags { "Optimize" }

   project "libopkele"

      language "C++"

      includedirs { "external/libopkele/include" } 

      files { "external/libopkele/lib/**.cc" }

      kind "StaticLib"

      configuration { "Debug" } 
         flags { "Symbols" } 

      configuration { "Release" } 
         flags { "Optimize" } 
