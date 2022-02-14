import { BaseController } from "./base_controller.js";

//The El checkbox element
class EnableController extends BaseController{
    constructor(theEl, theControlledFieldsList){
        super(theEl);
        this._m_BaseElement.onchange = this.update.bind(this); 
        this._m_CFields = theControlledFieldsList;
    }

    update(){
        let isChecked = this._m_BaseElement.checked;
        for (let i = 0; i < this._m_CFields.length; i++) {
            let anEl = this._m_CFields[i];
            if( anEl ){
                if( isChecked ){
                    anEl.classList.remove("disable");
                }
                else{
                    anEl.classList.add("disable");
                }
            }
        }
    }
};

export {EnableController};