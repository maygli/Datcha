import {BaseController} from "./base_controller.js"

class MenuController extends BaseController{
    constructor(theEl){
        super(theEl);
        this._m_Items = this._m_BaseElement.querySelectorAll('[data-controller_item="menu_item"]');
        for( let i = 0 ; i < this._m_Items.length ; i++ ){
            this._m_Items[i].addEventListener("click",this._onItemClicked.bind(this));
        }
    }

    viewMenu(isView){
        if( isView ){
            super.show();
            let anActiveItem = this._getActiveItem();
            if( anActiveItem )
                this._executeItem(anActiveItem);
        }
        else{
            super.hide();
        }
    }

    _getActiveItem(){
        for( let i = 0 ; i < this._m_Items.length ; i++ ){
            let aClassName = this._m_Items[i].className;
            if( aClassName.indexOf("active") >= 0 ){
                return this._m_Items[i];
            }
        }
        return null;
    }

    _onItemClicked(theEvent){
        let aSelItem = theEvent.currentTarget;
        this._selectItem(aSelItem);        
    }

    _selectItem(theItem){
        let anActiveItem = this._getActiveItem();
        if( anActiveItem != null ){
            this._deactivateItem(anActiveItem);
        }
        this._activateItem(theItem);
    }

    _activateItem(theItem){
        theItem.classList.add("active");
        this._executeItem(theItem);
    }

    _deactivateItem(theItem){
        theItem.classList.remove("active");
    }

    _executeItem(theItem){
    }

}

export {MenuController};