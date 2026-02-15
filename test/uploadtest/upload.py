#!/bin/python

from inventree.api import InvenTreeAPI
from inventree.part import Part, PartCategory


inventree_api = InvenTreeAPI("http://localhost:8000", token="inv-cfdeb7eb1b8a30c013ecf21db9870816955dc557-20250325")


def upload_part_datasheet(datasheet_url: str, part_ipn: int, part_pk: int, silent=False) -> str:
    ''' Upload InvenTree part attachment'''
    global inventree_api

    datasheet_name = f'{part_ipn}.pdf'

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


upload_part_datasheet("/tmp/TEMPO16_MODEL_2015_with V2.0 RX board with GND connector.pdf", "1N4148-DIO", 421)
