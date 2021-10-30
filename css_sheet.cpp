#include "css_sheet.h"
#include <stdio.h>

CssSheet::CssSheet() {
}

CssSheet::~CssSheet() {
}

void CssSheet::OnParseSelector(CssSelectorMode mode, const css_str_t* str, void* ud) {
	CssSheet* pThis = static_cast<CssSheet*>(ud);
	switch (mode) {
	case CssSelectorMode::kSelectorBegin:
		pThis->current_ = std::make_shared<CssStyles>();
		break;
	case CssSelectorMode::kSelectorValue:
		if (str->len && str->data[0] == '.') {
			pThis->class_styles_[std::string(str->data + 1, str->len - 1)] = pThis->current_;
		}
		else if (str->len && str->data[0] == '#') {
			pThis->id_styles_[std::string(str->data + 1, str->len - 1)] = pThis->current_;
		}
		else {
			pThis->element_styles_[std::string(str->data, str->len)] = pThis->current_;
		}
		break;
	case CssSelectorMode::kSelectorEnd:
		pThis->current_ = nullptr;
		break;
	}
}

void CssSheet::OnParseValue(const css_str_t* key, const css_str_t* value, void* ud) {
	CssSheet* pThis = static_cast<CssSheet*>(ud);
	if (pThis->current_) {
		(*pThis->current_)[std::string(key->data, key->len)] = std::string(value->data, value->len);
	}
}

bool CssSheet::Parse(const char* s) {
	return css_parse(s, OnParseSelector, OnParseValue,this);
}

bool CssSheet::Parse(const std::string& s) {
	return css_parse(s.c_str(), OnParseSelector, OnParseValue, this);
}

std::shared_ptr<CssStyles> CssSheet::GetStylesByClass(const std::string& key) const {
	auto find = class_styles_.find(key);
	if (find != class_styles_.end()) {
		return find->second;
	} else {
		return nullptr;
	}
}

std::shared_ptr<CssStyles> CssSheet::GetStylesById(const std::string& key) const {
	auto find = id_styles_.find(key);
	if (find != id_styles_.end()) {
		return find->second;
	} else {
		return nullptr;
	}
}

std::shared_ptr<CssStyles> CssSheet::GetStylesByElement(const std::string& key) const {
	auto find = element_styles_.find(key);
	if (find != element_styles_.end()) {
		return find->second;
	} else {
		return nullptr;
	}
}

bool CssSheet::Load(const char* file_path) {
	FILE* file = fopen(file_path, "rb");
	bool rslt = false;
	if (file) {
		fseek(file, 0, SEEK_END);
		const long size = ftell(file);
		fseek(file, 0, SEEK_SET);

		char* buf = (char*)malloc(size + 1);
		if (buf && fread(buf, size, 1, file)) {
			buf[size] = '\0';
			rslt = Parse(buf);
		}
		free(buf);
	}
	return rslt;
}

