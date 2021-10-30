#include "css_parser.h"
#include <string.h>

static const char *skip(const char *in) {
	if (!in)
		return nullptr;

	//跳过空格换行等字符
	while (*in && ((unsigned char)*in <= 32)) {
		in++;
	}

	//跳过注释
	if (in[0] == '/' && in[1] == '*') {
		in += 2;
		while (*in) {
			if (in[0] == '*' && in[1] == '/') {
				in += 2;
				break;
			}
			in++;
		}

		//跳过空格换行等字符
		while (*in && ((unsigned char)*in <= 32)) {
			in++;
		}
	}
	return in;
}

static const char *skip_bom(const char *in) {
	if (in[0] == 0xEF && in[1] == 0xBB && in[2] == 0xBF) {
		return in + 3;
	}
	return in;
}

static size_t value_len(const char *str, const char *ptr) {
	while (ptr > str && (unsigned char)*(ptr-1) <= 32) {
		--ptr;
	}
	return ptr - str;
}


//解析selector
static const char *parse_selector(const char *str, parse_selector_func func,void* ud) {
	func(CssSelectorMode::kSelectorBegin, nullptr, ud);
	if (*str == '{') {
		css_str_t cstr = { str,0 };
		func(CssSelectorMode::kSelectorValue,&cstr, ud);
		return str;
	}

	const char* selector = str;
	while (*str) {
		if (*str == ',') {
			css_str_t cstr = { selector ,value_len(selector,str) };
			func(CssSelectorMode::kSelectorValue, &cstr,ud);
			str = skip(str + 1);
			selector = str;
			continue;
		} else if (*str == '{') {
			css_str_t cstr = { selector ,value_len(selector,str) };
			func(CssSelectorMode::kSelectorValue, &cstr, ud);
			return str;
		}
		++str;
	}
	return nullptr;
}

//解析key
static const char *parse_key(const char *str, css_str_t* value) {
	if (*str == ':') {
		value->data = str;
		value->len = 0;
		return str;
	}
	const char* key = str;
	while (*str) {
		if (*str == ':') {
			value->data = key;
			value->len = value_len(key, str);
			return str;
		}
		++str;
	}
	return nullptr;
}

static const char *parse_value(const char *str, css_str_t* value) {
	if (*str == ';' || *str == '}') {
		value->data = str;
		value->len = 0;
		return str;
	}
	const char* key = str;
	while (*str) {
		if (*str == ';') {
			value->data = key;
			value->len = value_len(key, str);
			return str;
		} else if (*str == '}') {
			value->data = key;
			value->len = value_len(key, str);
			return str;
		}
		++str;
	}

	if (key < str) {
		value->data = key;
		value->len = value_len(key, str);
		return str;
	}
	return nullptr;
}


bool css_parse(const char* str, parse_selector_func selector_func, parse_value_func value_func, void* ud) {
	if (!str)
		return false;

	str = skip_bom(str);

	while (*str) {
		str = skip(str);

		//解析selector
		str = parse_selector(str, selector_func, ud);
		if (!str) {
			return false;
		}

		//跳过{
		if (*str == '{') {
			str = skip(str + 1);
		}

		while (*str) {
			css_str_t key,value;
			str = parse_key(str,&key);

			if (!str || *str != ':') {
				return false;
			}

			str = skip(str + 1);
			str = parse_value(str,&value);
			if (!str) {
				return false;
			}

			value_func(&key, &value, ud);

			if (*str == ';') {
				str = skip(str + 1);
			}
			if (*str == '}') {
				str = skip(str + 1);
				selector_func(CssSelectorMode::kSelectorEnd, nullptr, ud);
				break;
			}
		}

	}

	return true;
}


bool css_parse(const char* str, parse_value_func value_func, void* ud) {
	if (!str)
		return false;

	str = skip_bom(str);
	str = skip(str);
	while (*str) {
		css_str_t key, value;
		str = parse_key(str, &key);

		if (!str || *str != ':') {
			return false;
		}

		str = skip(str + 1);
		str = parse_value(str, &value);
		if (!str) {
			return false;
		}

		value_func(&key, &value, ud);

		if (*str == ';') {
			str = skip(str + 1);
		}
	}
	return true;
}


static const char *parse_param(const char *str, css_str_t* value) {
	if (*str == ',' || *str == ')') {
		value->data = str;
		value->len = 0;
		return str;
	}
	const char* key = str;
	while (*str) {
		if (*str == ',') {
			value->data = key;
			value->len = value_len(key, str);
			return str;
		}
		else if (*str == ')') {
			value->data = key;
			value->len = value_len(key, str);
			return str;
		}
		++str;
	}
	return nullptr;
}

#define PARAM_MAX 16

bool css_parse_func(const char* str, parse_func_func value_func, void* ud) {
	if (!str)
		return false;

	str = skip_bom(str);
	str = skip(str);
	css_str_t name = {str,0};

	//解析name
	while (*str) {
		if (*str == '(') {
			name.len = value_len(name.data,str);
			break;
		}
		++str;
	}

	if (*str == '\0') {
		name.len = value_len(name.data, str);
	}

	//跳过(
	if (*str == '(') {
		str = skip(str + 1);
	}

	//解析param
	css_str_t params[PARAM_MAX];
	size_t index = 0;

	while (*str) {
		str = parse_param(str, params + index);
		if (!str) {
			break;
		}

		if (++index == PARAM_MAX) {
			break;
		}

		if (*str == ',') {
			str = skip(str + 1);
		}
		if (*str == ')') {
			str = skip(str + 1);
			break;
		}
	}
	value_func(&name, params, index, ud);
	return true;
}

