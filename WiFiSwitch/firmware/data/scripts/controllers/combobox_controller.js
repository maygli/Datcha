import {BaseController} from "./base_controller.js"

class ComboBoxController extends BaseController{
    constructor(theEl){
        super(theEl);
        this._m_BaseElement.onmouseover = this._onMouseOver.bind(this);
        this._m_BaseElement.onmouseout = this._onMouseOut.bind(this);
        this._m_ComboBoxContent = this._m_BaseElement.querySelector('[data-controller_item="ComboBoxContent"]'); 
        this._m_ComboBoxButton = this._m_BaseElement.querySelector('[data-controller_item="ComboBoxButton"]');
        this._m_ComboBoxItems = this._m_BaseElement.querySelectorAll('[data-controller_item="ComboBoxItem"]');
        this._m_CurrItemData = null;
        for( let i = 0 ; i < this._m_ComboBoxItems.length ; i++ ){
            if( i == 0 ){
                this._m_CurrItemData = this._m_ComboBoxItems[i].getAttribute(ComboBoxController._m_ItemDataAttribute);
            }
            this._m_ComboBoxItems[i].onclick = this._onClickComboBoxItem.bind(this);
        }
    }

    setCurrentItem(theData){
        if( this._m_ComboBoxButton.getAttribute(ComboBoxController._m_ItemDataAttribute) == theData ){
            return;
        }
        let aQueryString = '[' + ComboBoxController._m_ItemDataAttribute + '="' + theData + '"]';
        let anItemEl = this._m_ComboBoxContent.querySelector(aQueryString);
        this._selectItem(anItemEl);
    }

    getCurrentItem(){
        return this._m_CurrItemData;
    }

    setChangeCallback(theCallback){
        this._m_ChangeCallback = theCallback;
    }

    _onMouseOver()
    {
        let aComboBoxContent = this._m_ComboBoxContent;
        if( aComboBoxContent.style.display != "block" ){
            aComboBoxContent.style.display = "block";
        }
    }

    _onMouseOut()
    {
        let aComboBoxContent = this._m_ComboBoxContent;
        if( aComboBoxContent.style.display != "none" ){
            aComboBoxContent.style.display = "none";
        }
    }

    _onClickComboBoxItem(event)
    {
        let aSelItem = event.target||event.srcElement;
        let aParent = aSelItem;
        while(aParent){
            if( aParent.getAttribute("data-controller_item") == "ComboBoxItem"){
                break;
            }
            aParent = aParent.parentElement;
        }
        this._selectItem(aParent);
    }

    _selectItem(theItem){
        this._m_ComboBoxContent.style.display = "none";
        let anAttrsToCopy = [ComboBoxController._m_ItemDataAttribute, ComboBoxController._m_TransAttribute];
        if( theItem ){
            this._m_ComboBoxButton.innerHTML = theItem.innerHTML;
            anAttrsToCopy.forEach(function(anAttr, i, anArr){
              let aData = theItem.getAttribute(anAttr);
              if( aData != null ){
                this._m_ComboBoxButton.setAttribute(anAttr,aData);
              }
            }, this);
            let anId = theItem.getAttribute(ComboBoxController._m_ItemDataAttribute);
            this._m_CurrItemData = anId;
            this._comboBoxItemSelected(anId);
            if( this._m_ChangeCallback ){
                this._m_ChangeCallback();
            }
        }
    }

    _comboBoxItemSelected(theData){
    }

    static _m_ItemDataAttribute = "data-controller_item_data";
    static _m_TransAttribute = "data-i18n";
}

export {ComboBoxController};