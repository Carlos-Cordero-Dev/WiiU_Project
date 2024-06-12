workspace "MyProject"
   architecture "x86_64"
   configurations { "Debug", "Release" }

   startproject "MyProject"

-- Set the location of the devkitPro folder here

DEVKITPRO = "D:/DevkitPro/"

project "MyProject"
   kind "Makefile"
   language "C++"

   targetdir "build/bin/%{cfg.buildcfg}"
   objdir "build/obj/%{cfg.buildcfg}"
   
   includedirs {
      DEVKITPRO .. "/wut/include",
      DEVKITPRO .. "/libogc/include",
      DEVKITPRO .. "/portlibs/ppc/include",
      "include"
   }

   -- Library directories for linking
   libdirs {
      DEVKITPRO .. "/wut/lib",
      DEVKITPRO .. "/libogc/lib",
      DEVKITPRO .. "/portlibs/ppc/lib"
   }

   -- Files to include in the project
   files {
      "source/**.cpp",
      "source/**.c",
      "include/**.h",
      "data/**"
   }

   -- Custom build commands
   buildcommands { "make" }
   rebuildcommands { "make clean", "make" }
   cleancommands { "make clean" }

   -- Custom debug command
   debugcommand "$(ProjectDir)\\tools\\run_cemu_and_tail.bat"


   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
