Trial to implement a little of MVVM techniques. 

Goals
-----

- Interchangeable view layer (UMG asset + code behind) ('plug-it-in, plug-it-out' functionality). 


Concept
-------

- Place View Model layer inside this folder (`Widgets/Model`). 
- Suppose View from `Widget` folder is named as you want to. 
- Create View Model class, name it like `FVM_...(PartOfNameOfViewClass)` or `FViewModel_...(PartOfNameOfViewClass)`. 
- Names of View and View Model should be visually coupled (`UEditorWidget_...` <-> `FViewModel_...`, or `UEW_...` <-> `FVM_...` for instance). 
- No constraints for View Model class inheritance, no frameworks yet. 
