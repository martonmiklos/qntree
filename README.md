### Generated code

1. Grab yaml API schema from Inventree (http://localhost:8000/api/schema/)
2. Run `npx @openapitools/openapi-generator-cli generate -i "schema.yaml" -g cpp-qt-client -o /home/mm/Projektek/inventree2/qntree/gen_src --skip-validate-spec --additional-properties=cppNamespace=InvenTree --additional-properties=modelNamePrefix=""`

