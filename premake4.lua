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

      includedirs { "external/libopkele/include" }
      libdirs { "../libopkele/lib/.libs" }

      files { "src/**.h", "src/**.cpp", "src/**.c" }

      links { "libopkele", "sqlite3", "curl", "expat", "ssl", "crypto", "z", "tidy" }

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
