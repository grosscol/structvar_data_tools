# Create testing data subsets for cram summarizer.
# Generate sam files from cram files for use as test data committed to git repo.
# 
#  Runtime expectations:
#    - on server with access to primary crams
#    - in directory where output sams will be made
#
# Paths to crams resolved using where.sh script (/net/topmed/working/home/topmed/where.sh)
#  E.g.
#  PROJECT=topmed ~/where.sh NWD000001 b38
#
# prefixes, genomic positions, and cram paths must be hard coded before use.

REF="/net/share/ftp/vt/grch38/hs38DH.fa"

# Args: ($1) prefix, ($2) range, ($3) array of crams
#   process_crams "DUP_1" "chr20:20200-20600" CRAM_ARR
function process_crams() { 
  local L_PREFIX=$(echo $1 | tr '[:upper:]' '[:lower:]')
  local OUT_FA="${L_PREFIX}.fa"
  shift
  local L_RANGE=$1
  local TMP_BED=/tmp/${L_RANGE}.bed
  shift
  local L_CRAMS=("$@")

  # Create temp bedfile and subset fasta
  echo $L_RANGE | tr ':-' '\t' > /tmp/${L_RANGE}.bed
  bedtools getfasta -fi ${REF} -bed ${TMP_BED} -fo ${OUT_FA}

  # Subset and convert cram to sams
  local L_IDX=1
  for CRAM in ${L_CRAMS[@]}; do
    local OUT_SAM="${L_PREFIX}_sample_${L_IDX}.sam"
    local OUT_CRAM="${L_PREFIX}_sample_${L_IDX}.cram"
    echo $OUT_SAM
    samtools view --with-header --reference ${REF} $CRAM ${L_RANGE} > ${OUT_SAM}
    samtools view --cram --reference ${REF} $CRAM ${L_RANGE} > ${OUT_CRAM}
    samtools index ${OUT_CRAM}

    let "L_IDX++"
  done
}

PREFIX=dup_1
RANGE=chr1:820200-823600
CRAMS=(\
  "/full/path/to/study/NWD000001/NWD000001.cram" \
  "/full/path/to/study/NWD000002/NWD000002.cram" \
  "/full/path/to/study/NWD000003/NWD000003.cram")
process_crams ${PREFIX} ${RANGE} ${CRAMS[@]}

PREFIX=dup_2
RANGE=chr1:50186563-50186750
CRAMS=(\
  "/full/path/to/study/NWD000001/NWD000001.cram" \
  "/full/path/to/study/NWD000002/NWD000002.cram" \
  "/full/path/to/study/NWD000003/NWD000003.cram" \
  "/full/path/to/study/NWD000004/NWD000004.cram" \
  "/full/path/to/study/NWD000005/NWD000005.cram" \
  "/full/path/to/study/NWD000006/NWD000006.cram" )
process_crams ${PREFIX} ${RANGE} ${CRAMS[@]}
