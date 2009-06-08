/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Languages.h"

#include <QtCore/QDebug>

Languages::Languages() {
}

Languages::~Languages() {
}

QMap<QString, QString> Languages::iso639_1_list() {
	static QMap<QString, QString> list;
	if (list.isEmpty()) {
		//Lazy initialization
		list["aa"] = tr("Afar");
		list["ab"] = tr("Abkhazian");
		list["ae"] = tr("Avestan");
		list["af"] = tr("Afrikaans");
		list["ak"] = tr("Akan");
		list["am"] = tr("Amharic");
		list["an"] = tr("Aragonese");
		list["ar"] = tr("Arabic");
		list["as"] = tr("Assamese");
		list["av"] = tr("Avaric");
		list["ay"] = tr("Aymara");
		list["az"] = tr("Azerbaijani");
		list["ba"] = tr("Bashkir");
		list["be"] = tr("Byelorussian");
		list["bg"] = tr("Bulgarian");
		list["bh"] = tr("Bihari");
		list["bi"] = tr("Bislama");
		list["bm"] = tr("Bambara");
		list["bn"] = tr("Bengali");
		list["bo"] = tr("Tibetan");
		list["br"] = tr("Breton");
		list["bs"] = tr("Bosnian");
		list["ca"] = tr("Catalan");
		list["ce"] = tr("Chechen");
		list["ch"] = tr("Chamorro");
		list["co"] = tr("Corsican");
		list["cr"] = tr("Cree");
		list["cs"] = tr("Czech");
		list["cu"] = tr("Church Slavic");
		list["cv"] = tr("Chuvash");
		list["cy"] = tr("Welsh");
		list["da"] = tr("Danish");
		list["de"] = tr("German");
		list["dv"] = tr("Divehi");
		list["dz"] = tr("Bhutani");
		list["ee"] = tr("Ewe");
		list["el"] = tr("Greek");
		list["en"] = tr("English");
		list["eo"] = tr("Esperanto");
		list["es"] = tr("Spanish");
		list["et"] = tr("Estonian");
		list["eu"] = tr("Basque");
		list["fa"] = tr("Persian");
		list["ff"] = tr("Fulah");
		list["fi"] = tr("Finnish");
		list["fj"] = tr("Fiji");
		list["fo"] = tr("Faroese");
		list["fr"] = tr("French");
		list["fy"] = tr("Frisian");
		list["ga"] = tr("Irish");
		list["gd"] = tr("Scottish Gaelic");
		list["gl"] = tr("Galician");
		list["gn"] = tr("Guaraní");
		list["gu"] = tr("Gujarati");
		list["gv"] = tr("Manx");
		list["ha"] = tr("Hausa");
		list["he"] = tr("Hebrew");
		list["hi"] = tr("Hindi");
		list["ho"] = tr("Hiri Motu");
		list["hr"] = tr("Croatian");
		list["ht"] = tr("Haitian");
		list["hu"] = tr("Hungarian");
		list["hy"] = tr("Armenian");
		list["hz"] = tr("Herero");
		list["ia"] = tr("Interlingua");
		list["id"] = tr("Indonesian");
		list["ie"] = tr("Interlingue");
		list["ig"] = tr("Igbo");
		list["ii"] = tr("Sichuan Yi");
		list["ik"] = tr("Inupiak");
		list["io"] = tr("Ido");
		list["is"] = tr("Icelandic");
		list["it"] = tr("Italian");
		list["iu"] = tr("Inuktitut");
		list["ja"] = tr("Japanese");
		list["jv"] = tr("Javanese");
		list["ka"] = tr("Georgian");
		list["kg"] = tr("Kongo");
		list["ki"] = tr("Kikuyu");
		list["kj"] = tr("Kwanyama");
		list["kk"] = tr("Kazakh");
		list["kl"] = tr("Kalaallisut");
		list["km"] = tr("Khmer");
		list["kn"] = tr("Kannada");
		list["ko"] = tr("Korean");
		list["kr"] = tr("Kanuri");
		list["ks"] = tr("Kashmiri");
		list["ku"] = tr("Kurdish");
		list["kv"] = tr("Komi");
		list["kw"] = tr("Cornish");
		list["ky"] = tr("Kirghiz");
		list["la"] = tr("Latin");
		list["lb"] = tr("Luxembourgish");
		list["lg"] = tr("Ganda");
		list["li"] = tr("Limburgish");
		list["ln"] = tr("Lingala");
		list["lo"] = tr("Lao");
		list["lt"] = tr("Lithuanian");
		list["lu"] = tr("Luba-Katanga");
		list["lv"] = tr("Latvian");
		list["mg"] = tr("Malagasy");
		list["mh"] = tr("Marshallese");
		list["mi"] = tr("Maori");
		list["mk"] = tr("Macedonian");
		list["ml"] = tr("Malayalam");
		list["mn"] = tr("Mongolian");
		list["mr"] = tr("Marathi");
		list["ms"] = tr("Malay");
		list["mt"] = tr("Maltese");
		list["my"] = tr("Burmese");
		list["na"] = tr("Nauru");
		list["nb"] = tr("Norwegian Bokmål");
		list["nd"] = tr("North Ndebele");
		list["ne"] = tr("Nepali");
		list["ng"] = tr("Ndonga");
		list["nl"] = tr("Dutch");
		list["nn"] = tr("Norwegian Nynorsk");
		list["no"] = tr("Norwegian");
		list["nr"] = tr("South Ndebele");
		list["nv"] = tr("Navajo");
		list["ny"] = tr("Chichewa");
		list["oc"] = tr("Occitan");
		list["oj"] = tr("Ojibwa");
		list["om"] = tr("Oromo");
		list["or"] = tr("Oriya");
		list["os"] = tr("Ossetian");
		list["pa"] = tr("Panjabi");
		list["pi"] = tr("Pali");
		list["pl"] = tr("Polish");
		list["ps"] = tr("Pashto");
		list["pt"] = tr("Portuguese");
		list["qu"] = tr("Quechua");
		list["rm"] = tr("Rhaeto-Romance");
		list["rn"] = tr("Kirundi");
		list["ro"] = tr("Romanian");
		list["ru"] = tr("Russian");
		list["rw"] = tr("Kinyarwanda");
		list["sa"] = tr("Sanskrit");
		list["sc"] = tr("Sardinian");
		list["sd"] = tr("Sindhi");
		list["se"] = tr("Northern Sami");
		list["sg"] = tr("Sango");
		list["si"] = tr("Sinhala");
		list["sk"] = tr("Slovak");
		list["sl"] = tr("Slovenian");
		list["sm"] = tr("Samoan");
		list["sn"] = tr("Shona");
		list["so"] = tr("Somali");
		list["sq"] = tr("Albanian");
		list["sr"] = tr("Serbian");
		list["ss"] = tr("Swati");
		list["st"] = tr("Southern Sotho");
		list["su"] = tr("Sundanese");
		list["sv"] = tr("Swedish");
		list["sw"] = tr("Swahili");
		list["ta"] = tr("Tamil");
		list["te"] = tr("Telugu");
		list["tg"] = tr("Tajik");
		list["th"] = tr("Thai");
		list["ti"] = tr("Tigrinya");
		list["tk"] = tr("Turkmen");
		list["tl"] = tr("Tagalog");
		list["tn"] = tr("Tswana");
		list["to"] = tr("Tonga");
		list["tr"] = tr("Turkish");
		list["ts"] = tr("Tsonga");
		list["tt"] = tr("Tatar");
		list["tw"] = tr("Twi");
		list["ty"] = tr("Tahitian");
		list["ug"] = tr("Uighur");
		list["uk"] = tr("Ukrainian");
		list["ur"] = tr("Urdu");
		list["uz"] = tr("Uzbek");
		list["ve"] = tr("Venda");
		list["vi"] = tr("Vietnamese");
		list["vo"] = tr("Volapük");
		list["wa"] = tr("Walloon");
		list["wo"] = tr("Wolof");
		list["xh"] = tr("Xhosa");
		list["yi"] = tr("Yiddish");
		list["yo"] = tr("Yoruba");
		list["za"] = tr("Zhuang");
		list["zh"] = tr("Chinese");
		list["zu"] = tr("Zulu");
	}
	return list;
}

QMap<QString, QString> Languages::availableTranslations() {
	static QMap<QString, QString> list;
	if (list.isEmpty()) {
		//Lazy initialization
		QMap<QString, QString> tmp = iso639_1_list();
		list["en"] = tmp.value("en");
		list["fr"] = tmp.value("fr");
		list["de"] = tmp.value("de");
		list["es"] = tmp.value("es");
	}
	return list;
}
