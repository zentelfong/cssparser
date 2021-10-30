project "cssparser"
	kind "StaticLib"
	language "C++"
	files{
		"*.h",
		"*.cpp",
	}

--[[
project "css_test"
	kind "ConsoleApp"
	language "C++"
	links{
		"cssparser"
	}
	files{
		"test/*.cpp"
	}	
]]

