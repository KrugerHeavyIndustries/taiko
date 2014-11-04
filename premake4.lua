solution "taikod"
  
   newoption { 
      trigger = "no-pam", 
      description = "Use crypt and system password file directly (for systems without pam)"
   }

   configurations { "Debug", "Release" }

   platforms { "native", "x32", "x64" }

   location("build")

   configuration "Debug"
      targetdir "bin/debug"

   configuration "Release"
      targetdir "bin/release"

   configuration "no-pam"
      defines { "SHADOW_NONE" }

   project "taikod" 
      
      language "C++"

      includedirs { "external/libopkele/include", "/usr/local/include" }
      libdirs { "/usr/local/lib" }

      files { "src/**.h", "src/**.cpp", "src/**.c" }

      links { 
         "opkele", "sqlite3", "curl", "expat", "ssl", "crypto", "z", "tidy", 
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
	 defines { "PAM" }
         libdirs { "/lib/i386-linux-gnu", "/usr/lib/i386-linux-gnu" }
	 links { "dl", "uuid", "pthread", "pam" }

   project "opkele"

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
