import { BaseController } from "./base_controller.js";

class MultiStateImage extends BaseController{
    constructor(theEl, theImages){
        super(theEl);
        this._m_Images = theImages;
        this._m_BaseElement.addEventListener("click",this._onClick.bind(this));
        this.setState(0);
    }

    setState(theState){
        if( (theState < 0 ) || (theState >= this._m_Images.length) )
            return;
        this._m_State = theState;
        this._m_BaseElement.setAttribute("src", this._m_Images[theState]);
    }

    getState(){
        return this._m_State;
    }

    setChangeCallback(theCallback){
        this._m_ChangeCallback = theCallback;
    }

    _onClick(){
        this._m_State++;
        if( this._m_State >= this._m_Images.length )
            this._m_State = 0;
        this.setState(this._m_State);
        if( this._m_ChangeCallback ){
            this._m_ChangeCallback();
        }
    }
};

export {MultiStateImage};