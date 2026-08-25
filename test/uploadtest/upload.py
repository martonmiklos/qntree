#!/bin/python

from inventree.api import InvenTreeAPI
from inventree.part import Part, PartCategory


inventree_api = InvenTreeAPI("http://localhost:8000", token="inv-cfdeb7eb1b8a30c013ecf21db9870816955dc557-20250325")


def upload_part_datasheet(datasheet_url: str, part_pk: int, silent=False) -> str:
    ''' Upload InvenTree part attachment'''
    global inventree_api

    # Upload Datasheet to InvenTree
    part = Part(inventree_api, part_pk)
    if part:
        try:
            attachment = part.uploadAttachment(attachment=datasheet_url)
            return f'{inventree_api.base_url.strip("/")}{attachment["attachment"]}'
        except Exception:
            return ''
    else:
        return ''
f = open("/tmp/1.txt", "w")
f.write("Now the file has more content!")
f.close()

upload_part_datasheet("/tmp/1.txt", 1)
