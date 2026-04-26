# QnTree

## What is it?

A desktop tool to import part data into InvenTree from various sources.

It is going to be tailored to remove repeated tasks for part data import
* Automated parameter mapping
* Automated import part category - InvenTree part category mapping

### Supported data sources

* TME
* Mouser

### Planned data sources

* Data sources supported by Ki-n-tree

## Why not Ki-n-tree?

Serious desktop tools needs serious UI tookits and I cannot consider Flet to a serious toolkit. 

A framework what shipped with broken basic functionalities like the lack of "select - paste with middle click on Linux" feature is a no go for me.

## How?

### Generated code

The InvenTree API is generated

1. Grab yaml API schema from InvenTree for e.g. (http://localhost:8000/api/schema/)
2. Run `npx @openapitools/openapi-generator-cli generate -i "schema.yaml" -g cpp-qt-client -o /home/mm/Projektek/inventree2/qntree/gen_src --skip-validate-spec --additional-properties=cppNamespace=InvenTree --additional-properties=modelNamePrefix=""`

