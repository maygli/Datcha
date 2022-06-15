import {HttpProcessor} from "./http_processor.js"
import { Translator } from "./translator.js"
import {StackController} from "./controllers/stack_controller.js"
import {PageController,ParametersPageController,FormPageController} from "./controllers/page_controller.js"
import {SwitchSettingsPageController} from "./settings_switch.js"
import {WiFiSettingsPageController} from "./settings_wifi.js"
import {AboutPageController} from "./settings_about.js"
import {UpgradeController} from "./settings_upgrade.js"
import {MainMenu} from "./main_menu.js"

export async function onInit()
{
//  let language = navigator.languages ? navigator.languages[0] : (navigator.language || navigator.userLanguage);
  let language = "en";
  i18next.init({
      lng: language,
      debug: true,
      resources: translations 
  });
  let aLang = language.split("-")[0];
  Translator.translate(aLang, document);

  let aBoardInfo = await HttpProcessor.loadData("board_info",false);
  setMainWindowBoardInfo(aBoardInfo);

  let aPagesContent = document.querySelector("#pages");
  let aPagesStack = new StackController(aPagesContent);

  let aMenuEl = document.querySelector("#main_menu");
  let aMenu = new MainMenu(aMenuEl, aPagesStack);

  let aPageGeneralEl = document.querySelector("#page_general");
  let aMainPageController = new SwitchSettingsPageController(aPageGeneralEl);
  aPagesStack.addPageController("menu_general",aMainPageController);

  let aPageWifiEl = document.querySelector("#page_wifi");
  let aWiFiPageController = new WiFiSettingsPageController(aPageWifiEl);
  aPagesStack.addPageController("menu_wifi",aWiFiPageController);

  let aPageUpgradeEl = document.querySelector("#page_upgrade");
  let aPageUpgradeController = new UpgradeController(aPageUpgradeEl);
  aPagesStack.addPageController("menu_upgrade",aPageUpgradeController);

  let aPageAboutEl = document.querySelector("#page_about");
  let aPageAboutController = new AboutPageController(aPageAboutEl);
  aPagesStack.addPageController("menu_about",aPageAboutController);

  aPagesStack.activatePage("menu_general");
  console.log("Settings. onInit called");
}

function setMainWindowBoardInfo(theBoardInfo)
{
    let boardname = document.getElementById('boardname_id');
    boardname.innerHTML = theBoardInfo.board_name;
    let hwversion = document.getElementById("hardware_version_id")
    hwversion.innerHTML = theBoardInfo.board_hw_version;
    let aCopyRight = document.getElementById("main_footer_id")
    aCopyRight.innerHTML = theBoardInfo.copyright;
}




