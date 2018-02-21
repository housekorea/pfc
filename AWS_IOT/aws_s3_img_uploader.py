# -*- coding: utf-8 -*-
#!/bin/bash
import os
import sys
import boto
import boto.s3.connection
from boto.s3.key import Key
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from configure import pfc_conf

try:

    conn = boto.s3.connect_to_region(
        pfc_conf.PFC_AWS_S3_REGION,
        aws_access_key_id = pfc_conf.PFC_AWS_S3_ACCESS_KEY,
        aws_secret_access_key = pfc_conf.PFC_AWS_S3_SECRET_KEY,
        calling_format = boto.s3.connection.OrdinaryCallingFormat(),
    )








    bucket = conn.get_bucket(pfc_conf.PFC_AWS_S3_BUCKET_NAME)
    path = pfc_conf.PFC_AWS_S3_DIR_PATH #Directory Under which file should get upload

    uploadFileNames = []
    for (sourceDir, dirname, filename) in os.walk(pfc_conf.IMGS_PATH):
        uploadFileNames.extend(filename)
        break

    def percent_cb(complete, total):
        sys.stdout.write('.')
        sys.stdout.flush()

    for filename in uploadFileNames:
        sourcepath = os.path.join(pfc_conf.IMGS_PATH + filename)
        destpath = os.path.join(pfc_conf.PFC_AWS_S3_DIR_PATH, filename)
        print 'Uploading %s to Amazon S3 bucket %s' % \
               (sourcepath, bucket_name)

        filesize = os.path.getsize(sourcepath)
        if filesize > MAX_SIZE:
            print "multipart upload"
            mp = bucket.initiate_multipart_upload(pfc_conf.PFC_AWS_S3_DIR_PATH)
            fp = open(pfc_conf.PFC_AWS_S3_DIR_PATH,'rb')
            fp_num = 0
            while (fp.tell() < filesize):
                fp_num += 1
                print "uploading part %i" %fp_num
                mp.upload_part_from_file(fp, fp_num, cb=percent_cb, num_cb=10, size=PART_SIZE)
            mp.complete_upload()
        else:
            print "singlepart upload"
            k = boto.s3.key.Key(bucket)
            k.key = pfc_conf.PFC_AWS_S3_DIR_PATH
            k.set_contents_from_filename(pfc_conf.IMGS_PATH,
                    cb=percent_cb, num_cb=10)


except Exception,e:
    print str(e)
    print "error"
