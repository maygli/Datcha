import { MenuController } from "./controllers/menu_controller.js";

class MainMenu extends MenuController{
    constructor(theEl, thePageStack){
        super(theEl);
        this._m_PageStack = thePageStack;
    }

    _executeItem(theItem){
        let aPageId = theItem.id;
        this._m_PageStack.activatePage(aPageId);
    }
}

export {MainMenu};