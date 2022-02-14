import {BaseController} from "./base_controller.js"

class SliderController extends BaseController{
    constructor(theEl){
        super(theEl);
        this._m_Range = theEl.querySelector(".slider_range");
        this._m_Input = theEl.querySelector(".slider_input");
        this._m_ChangeCallback = null;
        if( this._m_Range ){
            this._m_Range.addEventListener("input",this._onSliderInput.bind(this));
            this._m_Range.addEventListener("change",this._onSliderChanged.bind(this));
        }
        if( this._m_Input ){
            this._m_Input.addEventListener("input",this._onInputInput.bind(this));
        }
        if( this._m_Range && this._m_Input ){
            this._m_Input.value = this._m_Range.value;
            this._m_Input.min = this._m_Range.min;
            this._m_Input.max = this._m_Range.max;
        }
    }

    setMin(theMin){
        this._m_Range.min = theMin;
        this._m_Input.min = theMin;
    }

    setMax(theMax){
        this._m_Range.max = theMax;
        this._m_Input.max = theMax;
    }

    setValue(theVal){
        if( ( theVal >= this._m_Range.min ) && (theVal <= this._m_Range.max) ){
            this._m_Range.value = theVal;
            this._m_Input.value = theVal;
        }
    }

    setChangeCallback(theCallback){
        this._m_ChangeCallback = theCallback;
    }

    getValue(){
        return this._m_Range.value;
    }

    _onSliderInput(){
        if( this._m_Range && this._m_Input ){
            this._m_Input.value = this._m_Range.value;
        }
    }

    _onSliderChanged(){
        this._onSliderInput();
        if( this._m_ChangeCallback ){
            this._m_ChangeCallback();
        }
    }

    _onInputInput(){
        if( this._m_Input && this._m_Range ){
            if(this._m_Input.checkValidity()){
                this._m_Range.value = this._m_Input.value;
                if( this._m_ChangeCallback ){
                    this._m_ChangeCallback();
                }
            }
            else{
                this._m_Input.reportValidity();
            }
        }
    }
}

export {SliderController};