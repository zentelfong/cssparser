#pragma once
#include <memory>
#include <map>
#include <string>
#include "css_parser.h"

typedef std::map<std::string, std::string> CssStyles;

class CssSheet {
public:
	CssSheet();
	~CssSheet();

	bool Parse(const char* s);
	bool Parse(const std::string& s);
	bool Load(const char* file);

	std::shared_ptr<CssStyles> GetStylesByClass(const std::string& key) const;
	std::shared_ptr<CssStyles> GetStylesById(const std::string& key) const;
	std::shared_ptr<CssStyles> GetStylesByElement(const std::string& key) const;
private:
	static void OnParseSelector(CssSelectorMode mode,const css_str_t* str, void* ud);
	static void OnParseValue(const css_str_t* key, const css_str_t* value, void* ud);

	std::shared_ptr<CssStyles> current_;
	std::map<std::string,std::shared_ptr<CssStyles>> class_styles_;//.a{}
	std::map<std::string,std::shared_ptr<CssStyles>> id_styles_;//#a{}
	std::map<std::string,std::shared_ptr<CssStyles>> element_styles_;//a {}
};

