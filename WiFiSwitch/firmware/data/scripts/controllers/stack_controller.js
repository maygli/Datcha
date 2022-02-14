import {BaseController} from "./base_controller.js"

class StackController extends BaseController{
    constructor( theEl ){
        super(theEl);
        this._mActiveItem = null;
        this._m_Pages = new Map();
    }

    addPageController( theId, theController ){
        this._m_Pages.set(theId, theController);
        theController.hide();
    }

    activatePage(theId){
        if( this._m_ActiveItem != null ){
            this._m_ActiveItem.hide();
        }
        let aNewActive = this._m_Pages.get(theId);
        if( aNewActive ){
            aNewActive.show();
            this._m_ActiveItem = aNewActive;
        }
    }
}

export {StackController};