/*

Copyright (c) 2015 Oliver Lau <ola@ct.de>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#include "stdafx.h"

#include <ShlObj.h>

namespace Impact {

  Settings gSettings;

  Settings::Settings(void)
    : useShaders(ENABLE_SHADERS)
    , verticalSync(false)
    , antialiasingLevel(16U)
    , particlesPerExplosion(50)
    , lastCampaignLevel(1)
    , musicVolume(50)
    , soundfxVolume(100)
  {
    TCHAR szPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath))) {
      this->appData = szPath;
      this->appData += "\\Impact";
      this->settingsFile = this->appData + "\\settings.xml";
      this->levelsDir = this->appData + "\\levels";
#ifndef NDEBUG
      std::cout << "settingsFile = '" << settingsFile << "'" << std::endl;
#endif
      load();
    }
  }


  bool Settings::save(void)
  {
    bool ok = true;
#ifndef NDEBUG
    std::cout << "Settings::save()" << std::endl;
#endif
    boost::property_tree::ptree pt;
    pt.put("impact.use-shaders", this->useShaders);
    pt.put("impact.use-shaders-for-explosions", this->useShadersForExplosions);
    pt.put("impact.vertical-sync", this->verticalSync);
    pt.put("impact.particles-per-explosion", this->particlesPerExplosion);
    pt.put("impact.antialiasing-level", this->antialiasingLevel);
    pt.put("impact.last-open-dir", this->lastOpenDir);
    pt.put("impact.campaign.last-level", this->lastCampaignLevel);
    pt.put("impact.music-volume", this->musicVolume);
    pt.put("impact.soundfx-volume", this->soundfxVolume);
    try {
      boost::property_tree::xml_parser::write_xml(settingsFile, pt);
    }
    catch (const boost::property_tree::xml_parser::xml_parser_error &ex) {
      std::cerr << "XML parser error: " << ex.what() << " (line " << ex.line() << ")" << std::endl;
      ok = false;
    }
    return ok;
  }


#pragma warning(disable : 4503)
  bool Settings::load(void)
  {
    bool ok = fileExists(settingsFile);
    if (!ok)
      return true;
    boost::property_tree::ptree pt;
    try {
      boost::property_tree::xml_parser::read_xml(settingsFile, pt);
    }
    catch (const boost::property_tree::xml_parser::xml_parser_error &ex) {
      std::cerr << "XML parser error: " << ex.what() << " (line " << ex.line() << ")" << std::endl;
      ok = false;
    }
    if (!ok)
      return false;

    try {
      this->useShaders = pt.get<bool>("impact.use-shaders", true);
      this->useShadersForExplosions = pt.get<bool>("impact.use-shaders-for-explosions", true);
      this->verticalSync = pt.get<bool>("impact.vertical-sync", false);
      this->particlesPerExplosion = pt.get<unsigned int>("impact.particles-per-explosion", 50U);
      this->antialiasingLevel = pt.get<unsigned int>("impact.antialiasing-level", 0U);
      this->lastOpenDir = pt.get<std::string>("impact.last-open-dir", levelsDir);
      this->lastCampaignLevel = pt.get<int>("impact.campaign.last-level", 1);
      if (this->lastCampaignLevel < 1)
        this->lastCampaignLevel = 1;
      this->soundfxVolume = b2Clamp(pt.get<float>("impact.soundfx-volume", 100), 0.f, 100.f);
      this->musicVolume = b2Clamp(pt.get<float>("impact.music-volume", 50), 0.f, 100.f);
    }
    catch (const boost::property_tree::xml_parser::xml_parser_error &ex) {
      std::cerr << "XML parser error: " << ex.what() << " (line " << ex.line() << ")" << std::endl;
      ok = false;
    }

#ifndef NDEBUG
    std::cout << "use-shaders: " << this->useShaders << std::endl;
    std::cout << "use-shaders-for-explosions: " << this->useShadersForExplosions << std::endl;
    std::cout << "vertical-sync: " << this->verticalSync << std::endl;
    std::cout << "particles-per-explosion: " << this->particlesPerExplosion << std::endl;
    std::cout << "antialiasing-level: " << this->antialiasingLevel << std::endl;
    std::cout << "last-open-dir: " << this->lastOpenDir << std::endl;
    std::cout << "campaign.last-level: " << this->lastCampaignLevel << std::endl;
    std::cout << "impact.soundfx-volume: " << this->soundfxVolume << std::endl;
    std::cout << "impact.music-volume: " << this->musicVolume << std::endl;
#endif

    this->useShaders &= sf::Shader::isAvailable();
    this->useShadersForExplosions &= this->useShaders;

    return ok;
  }

}
