#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>

#include "../css_parser.h"
#include "../css_sheet.h"

void parse_selector(CssSelectorMode mode, const css_str_t* str, void* ud) {
	if(str)
		std::cout << std::string(str->data, str->len) << std::endl;
}

void parse_value(const css_str_t* key, const css_str_t* value, void* ud) {

	std::cout << std::string(key->data, key->len) 
		<< "="<< std::string(value->data, value->len)
		<< std::endl;
}


int main() {

	{
		const char* css1 = "{:;}";
		bool rslt = css_parse(css1, parse_selector, parse_value, NULL);
	}

	{
		const char* css1 = "{:}";
		bool rslt = css_parse(css1, parse_selector, parse_value, NULL);
	}

	{
		const char* css1 = "{}";
		bool rslt = css_parse(css1, parse_selector, parse_value, NULL);
	}

	{
		const char* css1 = "{;}";
		bool rslt = css_parse(css1, parse_selector, parse_value, NULL);
	}

	{
		const char* css1 = "/*test*/{/*test*/a:1;/*test*/}/*test*/";
		bool rslt = css_parse(css1, parse_selector, parse_value, NULL);
	}

	{
		const char* css1 = "p {color:red;/*test*/\ntext-align:center;}";
		css_parse(css1, parse_selector, parse_value, NULL);
	}

	{
		const char* css1 = "  a  {   b   :  b  ;  }  ";
		bool rslt = css_parse(css1, parse_selector, parse_value, NULL);
	}

	const char* css2 = ".a, .b  ,  #c { color : red ; text-align : center ; } #b {color:red;text-align:center;}";
	css_parse(css2, parse_selector, parse_value,NULL);


	CssSheet sheet;
	sheet.Parse(".a,#a,a{a1:1;a2:2}");

	getchar();
	return 0;
}