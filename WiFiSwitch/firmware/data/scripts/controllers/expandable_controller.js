import {BaseController} from "./base_controller.js"

class ExpandableController extends BaseController{
    constructor(theEl){
        super(theEl);
        this._m_Button = this._m_BaseElement.querySelector('[data-controller_item="ExpandButton"]');
        this._m_Button.onclick = this._onExpandClick.bind(this);
        this._m_ExpSign = this._m_BaseElement.querySelector( '[data-controller_item="ExpandSign"]' );
        this._m_CollapseSign = this._m_BaseElement.querySelector( '[data-controller_item="CollapseSign"]' );
        this._m_Content = this._m_BaseElement.querySelector( '[data-controller_item="ModalContent"]' );
        this.collapse();
    }

    _onExpandClick(){
        if( this.isExapnded() )
            this.collapse();
        else{
            this.expand();
        }
    }

    isExapnded(){
        return !this._m_Content.hidden;
    }

    collapse(){
        this._m_Content.hidden = true;
        this._m_ExpSign.hidden = false;
        this._m_CollapseSign.hidden = true;
    }

    expand(){
        this._m_Content.hidden = false;
        this._m_ExpSign.hidden = true;
        this._m_CollapseSign.hidden = false;
    }
}

export {ExpandableController};